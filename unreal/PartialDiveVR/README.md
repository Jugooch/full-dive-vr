# PartialDiveVR (Unreal project) — specification

This folder will hold the Unreal project once it is created (see [`../README.md`](../README.md)).
Until then this file is the spec the project must implement. It is derived from
[`docs/research/04-research-program-guide.md`](../../docs/research/04-research-program-guide.md) and
the experiment protocols in [`experiments/`](../../experiments/).

## Plugin: `PartialDiveBridge` (C++, Blueprint-exposed)

A `UGameInstanceSubsystem` that owns all I/O with the Python services (contracts in [`/schemas`](../../schemas)).

| Direction | What | Transport |
|---|---|---|
| in | `IntentFrame` (latest-wins; expose as a Blueprint struct + `OnIntentFrame` event) | UDP `127.0.0.1:47800` |
| in | `BlockStart` (`block/1`) → `OnBlockStart(Code, Params)` | UDP `127.0.0.1:47803` |
| in | `ChantPhrase` (V4) → `OnChantPhrase` | UDP `127.0.0.1:47802` |
| out | `HapticEvent` (`EmitHaptic(Kind, Zone, Strength, DurationMs, Path, StepMs)`) | UDP `127.0.0.1:47801` |
| out | game markers (trial start/end, cue, collision, cast) | LSL `pdive.game` |

Requirements:
- Timestamps on outgoing events use the **LSL clock** (`lsl_local_clock`) so the haptic bus can compute
  delays and the XDF lines up.
- Stale-intent safety: if no `IntentFrame` arrives for 250 ms, treat all intents as 0 (avatar stops).
- An **input-source switch** (`joystick | intent`) at runtime, so baselines (006 joystick, 103 button)
  use the same scene.

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
