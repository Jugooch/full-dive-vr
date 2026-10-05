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

## Content plan

| Asset | Used by | Notes |
|---|---|---|
| `Maps/Lab` | 000–004, 009 | One small laboratory room: mirror, table, cube, sphere, sword, target dummy. Not a game. |
| `Maps/Course` | 005–007, 201 | Fixed course: start → walk 10 m → turn left → around obstacle → approach table → stop in circle |
| `Maps/Forest` | 008 | Windy forest → pick up sword → strike shield → distant explosion (ablation scenario) |
| `Maps/ManaChamber` | 101–107 | Calm space for Sense → Gather → Route → Shape → Charge → Release |
| `Maps/Portals` | 012 | Left/right portal / hand-glow BCI feedback |
| `Avatar/BP_FullBodyAvatar` | all | **Full body** incl. hands and feet (not floating controller hands); camera at avatar eye position; reclined-friendly calibration |
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
