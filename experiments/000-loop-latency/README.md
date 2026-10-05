# 000 — End-to-end loop latency

- **Status:** planned
- **Version / Phase:** V0 (Phase 0 platform)
- **Branch:** Embodied Control
- **Depends on:** —
- **Research source:** [07-version-roadmap](../../docs/research/07-version-roadmap.md), [04-research-program-guide](../../docs/research/04-research-program-guide.md)

## Question

How long does the full V0 loop take, body → sensor → decoder → `IntentFrame` → Unreal → haptic command → actuator → body, and where does that time go?

## Hypothesis

The loop can run end to end with a median under **150 ms** from muscle onset to virtual response (the Phase 2 engineering target), and **20 ms or less** from virtual collision to actuator onset (the synchronized-touch target from Phase 1B). Every stage can be measured from LSL timestamps.

*(Procedural choice: this experiment isn't named in the research. It turns V0's "prove the loop" and the command-latency metric into a measurement, so every later experiment knows its latency budget.)*

## Hardware

- PCVR headset (Quest-class over Link is fine)
- ESP32 + 1× MyoWare 2.0 (forearm) + DRV2605L + 1 vibration motor
- Optional: a photodiode on the headset lens and a piezo/accelerometer on the actuator, read by a second ESP32 channel, to time display and motor onsets physically

## Software

- `partialdive` acquisition (serial → LSL), threshold decoder, Intent Bus publisher, Haptic Bus
- Unreal: test scene with one virtual hand that closes on `grab_right` and fires a `HAPTIC` event on close
- LSL markers at every hop; LabRecorder → XDF

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | EMG → hand → haptic | input: emg, haptics_enabled: true |
| B | Keyboard → hand → haptic (reference) | input: keyboard, haptics_enabled: true |

## Procedure

1. Start all LSL streams and LabRecorder, and confirm clock sync (LSL time-sync offsets logged).
2. Condition A: 50 brief forearm contractions, at least 3 s apart. Each fires grab → haptic.
3. Condition B: 50 key presses at the same pace.
4. Optionally repeat with the photodiode/accelerometer attached to get physical ground truth.

## Measures

- Per-stage latency: sensor sample → decoder output → IntentFrame received in Unreal → frame rendered → haptic command sent → actuator onset
- Median, p95 and jitter per stage ([metrics](../../docs/metrics.md): *command latency*)

## Success criteria / gate

- Median EMG-onset → virtual-response latency **< 150 ms**, p95 recorded
- Median collision → actuator onset **≤ 20 ms**
- Every hop is timestamped in one XDF file. This gate opens 002 and 003.

## Safety

See [safety](../../docs/safety.md). Battery-powered EMG electronics only while the electrodes are attached.

## Analysis plan

Load the XDF, align markers by LSL time and compute per-stage deltas. Plot latency histograms per stage. Re-run whenever the pipeline changes and record the result in `docs/research-log.md`.

## Notes / open questions

- Is BLE or serial faster and steadier from the ESP32? Measure both if wireless is planned.
- How much does the decoder's onset detection (envelope smoothing) add?
