# PartialDiveVR (Unreal project) — specification

The Unreal project is in [`PartialDiveVR/`](PartialDiveVR/) (setup: [`../README.md`](../README.md)).
This file is the spec the project implements. It is derived from
[`docs/research/04-research-program-guide.md`](../../docs/research/04-research-program-guide.md) and
the experiment protocols in [`experiments/`](../../experiments/).

## Plugin: `PartialDiveBridge` (implemented, v0.1.0)

Source: `PartialDiveVR/Plugins/PartialDiveBridge/`. A `UGameInstanceSubsystem` (+ tickable) that owns all
I/O with the Python services (contracts in [`/schemas`](../../schemas)). Get it in Blueprint with
**Get Game Instance Subsystem → PartialDiveBridgeSubsystem**.

| Direction | What | Transport | Blueprint |
|---|---|---|---|
| in | `intent-frame/1` | UDP `127.0.0.1:47800` | `GetIntent()` (poll each tick) or `OnIntentFrame` event |
| in | `block/1` | UDP `127.0.0.1:47803` | `OnBlockStart(Block)`, `GetBlockParamNumber/String/Bool(Key, Default)`, `GetActiveBlock().Code` |
| in | `chant-phrase/1` (V4) | UDP `127.0.0.1:47802` | `OnChantPhrase` |
| out | `haptic-event/1` | UDP `127.0.0.1:47801` | `EmitHaptic(Kind, Zone, Strength, DurationMs)`, `EmitHapticFlow(Path, ...)`, `StopAllHaptics()` |
| out | game markers | LSL `pdive.game` | `PushGameMarker(EventName, Fields)` → `{"event","t",...}` |
| — | shared clock | LSL | `GetLslClock()` |

Built in:
- **Stale-intent safety:** no `IntentFrame` for 250 ms → `GetIntent()` returns all zeros (avatar stops),
  `OnIntentStaleChanged` fires and an `intent_stale` / `intent_resumed` marker is pushed.
- **Input-source switch:** `SetInputSource(Intent | Controller)` for the joystick (006) and button (103)
  baselines in the same scene. In `Controller` mode, `GetIntent()` returns zeros and logs an `input_source` marker.
- Outgoing haptic timestamps use the LSL clock, so the haptic bus can apply delays and the XDF lines up.
- Ports and the 250 ms timeout are config: `Config/DefaultGame.ini` →
  `[/Script/PartialDiveBridge.PartialDiveBridgeSubsystem]` `IntentPort=…`, `StaleIntentSeconds=…`.
- Unit tests: `PartialDive.Json.*` (IntentFrame parsing incl. omitted zero channels, block, haptic JSON).

**Debug overlay:** console variable `pdive.debug` (default **1** in editor builds, 0 in packaged builds).
Shows intent LIVE/STALE + rate, all channels, block **code** (never params) and the last haptic event.
Network input is drained at the **start** of each world tick, so every actor sees the newest intent the
same frame it arrives.

Smoke test without hardware: run `partialdive dev` (repo root, PowerShell), press Play, and read the overlay.
Haptics: call `EmitHaptic(Contact, RightForearm)` from any Blueprint; the `dev` status line shows the
resulting actuator command.

## Avatar + lab (implemented, game module `Source/PartialDiveVR`)

Open the project and press **Play** (the editor now starts in `L_Lab`). With `partialdive dev` running you
are a full-body avatar driven by simulated intent; with the Rift S on, use **VR Preview**.

| Class | What it does |
|---|---|
| `APDAvatar` | Full-body first-person Manny. Hidden *driver* mesh runs `ABP_Unarmed` (idle/walk); the visible `UPDBodyMeshComponent` copies that pose every frame and adds head look, arm IK to the Touch controllers, and finger curl from grab. Camera at the avatar's eyes from the bind pose (no walk bob). |
| `APDGameMode` | Spawns `APDAvatar`. Set as the GameMode Override in `L_Lab`. |
| `APDProp` | Lab props from engine shapes: grabbable cube/sphere/sword (tag `PDGrabbable`, physics), table, target dummy. WorldDynamic, so they trigger touch haptics; floor and walls never do. |
| `APDMirror` | 120 × 200 cm mirror (scene capture reflected across the glass; planar reflections don't render with this project's forward shading/Substrate). Centre-eye correct; in VR both eyes see the same reflection. |

**Input.** `Intent` source (default): `walk_forward` → walking at up to 1.6 m/s, `turn_left/right` → 45°/s, `grab_left/right` → hand closure, grab ≥ 0.6 picks up the nearest
grabbable within 18 cm of the palm, ≤ 0.35 releases. `Controller` source (baselines, `I` toggles): left stick move,
right stick snap-turn 30°, grip = grab; keyboard W/A/S/D, Q/E turn, F/G grab. Block params `input`/`locomotion`
(`emg|eeg|fusion|intent` vs `keyboard|joystick|controller|button`) switch the source automatically.

**Reclined use.** ~1 s after Play (or **R**, or click both thumbsticks), `Recenter()` makes your current
physical gaze (pitch + yaw; roll ignored) the avatar's straight-ahead, so you can lie back and still face forward.

**Touch → haptics.** Invisible capsules on hands, forearms, upper arms, shoulders, chest, core, thighs and feet
emit `EmitHaptic(Contact, zone)` when a prop overlaps them (150 ms cooldown per zone, strength from relative speed).

**Arms.** `ArmSource = Auto`: arms follow the Touch controllers (two-bone IK) when tracked, otherwise the animation
(EMG experiments: hands at rest). Hand orientation comes from the bind-pose geometry, no bone-axis assumptions;
the fingers-along-controller-forward / palm-toward-midline mapping is a first guess and may need tuning in VR.

### Dev keys and console (`~`)

| Key / command | Effect |
|---|---|
| `V` / `pdive.view fp\|front\|back\|hand` | first person, debug cameras, right-hand close-up (cyan dot = eye point) |
| `R` / `pdive.recenter` | recenter view |
| `I` / `pdive.input intent\|controller` | switch input source |
| `pdive.avatar.grab <L> <R>` | force hand closure 0..1 (`-1` = real input) |
| `pdive.avatar.touchtest <zone>` | drop a ball through a body zone → haptic event (test motors without VR) |
| `pdive.avatar.handstats` | log fingertip geometry (relaxed vs shown) |
| `pdive.after <s> <command>` | run a command later (scripted tests/screenshots) |
| `pdive.debug 0\|1` | bridge overlay |

### Regenerating the lab

`L_Lab` and `M_PDMirror` are **generated** by [`PartialDiveVR/Scripts/build_lab.py`](PartialDiveVR/Scripts/build_lab.py). Edit the script, not the level:
```
UnrealEditor-Cmd.exe <PartialDiveVR.uproject> -ExecutePythonScript="<repo>\unreal\PartialDiveVR\PartialDiveVR\Scripts\build_lab.py" -unattended -nullrhi
```

### Verified headless (2026-10-05, no HMD)

Offscreen renders + logs: avatar renders and stands in the lab; first-person eye point; mirror shows the avatar
(correct handedness, matched exposure); simulated `walk_forward` walks the avatar; `pdive.avatar.grab` closes the
hand into a fist (index tip 15.4 → ~8 cm from the wrist, on the palm side); dropping balls on forearm/chest/hand
produced the expected `H <ch> …` commands on the haptic bus.

**Not yet verified (needs you in the headset):** reclined recentering, head look, Touch-controller arm IK and hand
orientation, snap turn, comfort and eye height. Tuning knobs are `Config=Game` properties on `APDAvatar`
(`EyeOffsetFromHead`, `FingerCurlDeg`, `GrabRadiusCm`, …).

## Content plan

| Asset | Used by | Notes |
|---|---|---|
| `Maps/Lab` | 000–004, 009 | ✅ `/Game/PartialDive/Levels/L_Lab`, generated by `Scripts/build_lab.py` |
| `Maps/Course` | 005–007, 201 | Fixed course: start → walk 10 m → turn left → around obstacle → approach table → stop in circle |
| `Maps/Forest` | 008 | Windy forest → pick up sword → strike shield → distant explosion (ablation scenario) |
| `Maps/ManaChamber` | 101–107 | Calm space for Sense → Gather → Route → Shape → Charge → Release |
| `Maps/Portals` | 012 | Left/right portal / hand-glow BCI feedback |
| `Avatar/BP_FullBodyAvatar` | all | ✅ done as C++ `APDAvatar` (see above) |
| `Avatar/ABP_IntentLocomotion` | 005+ | `walk_forward` → gait animation; avatar biomechanics come from animation/IK, not from the body |
| `Avatar/IK_Reach` | 004, 009, 013 | Intent ("reach for sword") → IK solves joints; never decode joint angles |
| `Mana/BP_ManaCore` | 101+ | Fixed point in the abdomen; glow visible through avatar, low spatial hum, synchronized breathing |
| `Mana/BP_ManaFlow` | 101–107 | Visual flow core → chest → shoulder → arm → hand, emits matching `flow` HapticEvent |
| `Mana/BP_SpellStateMachine` | 105+ | Sense/Gather/Route/Shape/Charge/Release; logs `mana-spell/1` |
| `UI/WBP_BlockCode` | all | Shows only the block code + questionnaire prompts (VEQ etc.) in-headset |

## Mana model (V2, from research)

- C = normalized core activation (`core_activation`), dM/dt = k·C; poor control/excess tension → inefficiency.
- D_R = w1·E_core + w2·E_rightarm + w3·R_breathing + w4·G_hand; above threshold → route flow to hand.
- **No breath-holding or hyperventilation** in any mechanic; normal comfortable breathing must suffice.
- Feedback over HUD: unstable routing → unstable vibration; too much power → aggressive core; correct
  formation → smooth coherent sensation. Percentages are for logs, not the player.
