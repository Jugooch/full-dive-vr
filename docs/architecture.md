# Architecture

Derived from the research guide's Phase 0 ([research/04](research/04-research-program-guide.md)) and
the "Intent Bus" design in [research/03](research/03-diy-research-strategy.md).

## The loop

```text
          ┌──────────────────────────────────────────────────────────────────┐
          │                              BODY                                │
          └───▲───────────────────────────────────────────────────────┬──────┘
              │ vibration / bass / airflow                            │ EMG · EEG · gaze · voice · breath
     ┌────────┴─────────┐                                   ┌─────────▼─────────┐
     │ firmware         │                                   │ firmware / vendor │
     │ haptics-controller│                                  │ emg-streamer,     │
     └────────▲─────────┘                                   │ BrainFlow boards  │
              │ haptic-command/1 (serial)                   └─────────┬─────────┘
     ┌────────┴─────────┐                                             │ samples
     │   HAPTIC BUS     │◄── block/1 (condition)          ┌───────────▼───────────┐
     │ delay · remap ·  │                                 │ biosignal → features  │
     │ gate · scale     │                                 │ → INTENT DECODER      │
     └────────▲─────────┘                                 └───────────┬───────────┘
              │ haptic-event/1 (UDP 47801)                            │ intent-frame/1 (UDP 47800)
     ┌────────┴───────────────────────────────────────────────────────▼───────────┐
     │                    UNREAL (PartialDiveVR + PartialDiveBridge)              │
     │  avatar · IK · locomotion · mana core · spell state machine · scenes       │
     └────────────────────────────────────────────────────────────────────────────┘
                 everything above also streams to LSL → LabRecorder → one XDF per session
```

## Principles

1. **Never send raw biosignals into gameplay logic.** Unreal sees `IntentFrame`s only. Joystick, EMG,
   EEG, gaze, fusion or a future BCI all produce the same frame (`source` says which). This is what lets
   the EEG track slot in without a rewrite. ([ADR 0003](decisions/0003-intent-bus.md))
2. **Decode intent, not joints.** The interface infers "reach for sword" plus rough direction, speed and grip;
   the engine's animation and IK supply the biomechanics. Shared autonomy (AI fills in imperfect
   commands) is the norm, not a fallback.
3. **Conditions live in the haptic bus, not in the game.** Unreal emits the *true* virtual event; the bus
   delays, remaps, scales or drops it per the blinded block params. One place to manipulate, one place
   to log.
4. **One clock.** Every timestamp is LSL `local_clock()`. LabRecorder captures every stream into one XDF
   per session. ([ADR 0004](decisions/0004-lsl-xdf-recording.md))
5. **Hardware details are data.** Which electrode is where and which motor is on which zone lives in
   `hardware/profiles/*.yaml`. Firmware knows channels, not body parts.
6. **Firmware is dumb and safe.** Hard caps (max on-time, watchdog, stop-all) are enforced on the MCU.

## Components

| Component | Where | Language | Status |
|---|---|---|---|
| Contracts | [`schemas/`](../schemas/) | JSON Schema | v1 defined |
| Biosignal acquisition | `services/src/partialdive/biosignal/` | Python | serial EMG + simulator |
| Intent decoding | `services/src/partialdive/intent/` | Python | threshold, continuous, cadence |
| Haptic bus | `services/src/partialdive/haptics/` | Python | conditions, flows, airflow |
| Session tooling | `services/src/partialdive/session/` + CLI | Python | init / block start / unblind |
| Analysis metrics | `services/src/partialdive/analysis/` | Python | control, latency, ITR, E/R |
| Voice / chants | `services/src/partialdive/voice/` | Python | V4 placeholder |
| EMG firmware | [`firmware/emg-streamer/`](../firmware/emg-streamer/) | C++ (Arduino) | v0.1 |
| Haptics firmware | [`firmware/haptics-controller/`](../firmware/haptics-controller/) | C++ (Arduino) | v0.1 |
| Unreal project + bridge | [`unreal/`](../unreal/) | UE 5.8 C++/BP | project created; `PartialDiveBridge` v0.1 + LSL plugin building, unit-tested |

## Contracts and ports

See [`schemas/README.md`](../schemas/README.md). Summary:

| Message | Port | LSL stream |
|---|---|---|
| `intent-frame/1` | UDP 47800 → Unreal | `pdive.intent` (+ raw `pdive.emg`) |
| `haptic-event/1` | UDP 47801 → haptic bus | `pdive.haptic` (dispatch log incl. lag) |
| `chant-phrase/1` | UDP 47802 → Unreal | `pdive.voice` |
| `block/1` | UDP 47803 → Unreal, 47801 → bus | `pdive.experiment` |
| game markers | — | `pdive.game` |

## Session workflow

Everyday development/testing is just `partialdive dev` + Play (overlay: `pdive.debug`). A recorded
experiment session adds calibration, LabRecorder and blinded blocks:

```bash
partialdive session init 002 --profile v0-forearm          # randomized, blinded block codes
partialdive calibrate --profile v0-forearm --out <session>/calibration.yaml
partialdive decode --profile v0-forearm --calibration <session>/calibration.yaml &
partialdive haptic-bus --profile v0-forearm &
# start LabRecorder (record all pdive.* streams), launch Unreal
partialdive block start <session> 1    # ... run block 1, answer questionnaire in-headset
partialdive block start <session> 2    # ... and so on
partialdive session unblind <session>  # only after all data is collected
```

## Signal pipelines by stage

**EMG (V0–V5)**: MyoWare ENV → ESP32 1 kHz → `ThresholdDetector` (T = μ_rest + kσ_rest with
hysteresis and dwell) / `ActivationNormalizer` (C ∈ [0,1]) / `StepCadence` (v = k·f_step) → IntentFrame.
From experiment 004 onward: windowed features (MAV, RMS, VAR, WL, peak) → logistic regression / LDA / random
forest. Deep learning only when the data justifies it.

**EEG (track)**: BrainFlow board → 60 Hz notch → μ (8–15 Hz) / β (16–31 Hz) band-pass → epoch around
cue → CSP → LDA → left/right. Split train/test by **run** and then by **day**, never by adjacent window.

**Fusion (Phase 6)**: X = [EEG, EMG, IMU, gaze, head orientation, scene affordances] → P(intent | X).
Gaze says *what*, EMG says *that a movement is starting*, EEG adds early *preparation*, and the scene says what's *possible*.

<a id="voice"></a>
**Voice (V4)**: headset mic → VAD → local recognizer (Picovoice Rhino speech-to-intent, or whisper.cpp)
→ phrase alignment against the chant grammar → `ChantPhrase` {spell, phase, confidence,
timing_error_ms}. Pronunciation stays forgiving. Engine choice gets an ADR when V4 starts.

## Data layout

See [`data/README.md`](../data/README.md). One directory per session; raw XDF is never committed.

## Extending

- **New intent:** add the field to `intent-frame.schema.json`, `contracts/intent.py` and the Unreal
  struct in one commit. Until then use `extra`. Promote a field once two experiments use it.
- **New sensor:** new firmware folder (if custom) or a BrainFlow board, plus a `biosignal` source, plus a profile.
- **New actuator type:** new `kind` in `haptic-event`, a firmware command, then handle it in `HapticBus`.
- **Breaking change:** bump the schema major version and write an ADR.
