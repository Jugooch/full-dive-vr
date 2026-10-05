# Schemas — the contracts between subsystems

These JSON Schemas are the **only** coupling between the signal side (Python services, firmware) and the
experience side (Unreal). Everything else can be rewritten independently as long as these hold.

| Schema | Producer → Consumer | Introduced |
|---|---|---|
| [`intent-frame.schema.json`](intent-frame.schema.json) | intent decoder → Unreal | V0 |
| [`haptic-event.schema.json`](haptic-event.schema.json) | Unreal → haptic bus | V0 |
| [`haptic-command.md`](haptic-command.md) | haptic bus → firmware (serial line protocol) | V0 |
| [`chant-phrase.schema.json`](chant-phrase.schema.json) | voice service → Unreal | V4 |
| [`mana-spell.schema.json`](mana-spell.schema.json) | Unreal spell state machine (logged) | V3 |
| [`block.schema.json`](block.schema.json) | `partialdive block start` → Unreal + haptic bus | V0 |
| [`session.schema.json`](session.schema.json) | `partialdive session init` → `data/sessions/**/session.yaml` | V0 |

Why this exists: the research is explicit that raw biosignals must never reach gameplay logic. The game
only ever sees an `IntentFrame`, whether it came from a joystick, EMG, EEG, eye tracking or a future BCI.
That is what lets the EEG track (Phases 5–6) slot in without rewriting V0–V6.
See [`docs/architecture.md`](../docs/architecture.md) and [ADR 0003](../docs/decisions/0003-intent-bus.md).

## Versioning rules

- Every message carries `schema` (e.g. `"intent-frame/1"`).
- **Additive changes** (new optional field, new enum value) keep the major version.
- **Breaking changes** (rename, remove, change meaning/range) bump the major version and need an ADR.
- Python mirrors live in `services/src/partialdive/contracts/`; Unreal mirrors live in the
  `PartialDiveBridge` plugin. Change all three in the same commit.

## Transport (localhost)

| Stream | Transport | Default |
|---|---|---|
| IntentFrame | UDP JSON datagrams + LSL stream `pdive.intent` | `127.0.0.1:47800` |
| HapticEvent | UDP JSON datagrams + LSL markers `pdive.haptic` | `127.0.0.1:47801` |
| ChantPhrase | UDP JSON datagrams + LSL markers `pdive.voice` | `127.0.0.1:47802` |
| BlockStart | UDP JSON to Unreal (and to the haptic bus on 47801) + LSL markers `pdive.experiment` | `127.0.0.1:47803` |
| Unreal experiment markers | LSL markers `pdive.game` | — |
