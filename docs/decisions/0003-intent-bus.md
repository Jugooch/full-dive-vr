# 0003 — Intent Bus: gameplay consumes only IntentFrames

- Status: accepted
- Date: 2026-10-05

## Context
"Never send raw biosignals directly into gameplay logic." The research proposes an `IntentFrame` struct
(timestamp, walk_forward, turn_left/right, grab_left/right, action_strength, confidence) so that EMG now
and EEG + EMG + eye tracking later produce the same frame: "this is what prevents Phase 5 from requiring
a rewrite" ([research/04](../research/04-research-program-guide.md), [research/03 §7](../research/03-diy-research-strategy.md)).

## Decision
`schemas/intent-frame.schema.json` (`intent-frame/1`) is the only body → game input. Fields are graded 0..1.
It's extended for synthetic physiology (`core_activation`, `route_left/right`, `release`,
`respiration_phase`, `stillness`) and has an `extra` map for experiment-specific channels.

## Consequences
Every input source (joystick baseline included) implements the same contract, so comparisons are fair.
Schema changes must update Python, Unreal and the schema together.
