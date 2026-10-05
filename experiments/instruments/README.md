# Instruments

These are the questionnaires used after experiment blocks. Each `conditions.yaml` lists the ones it needs under `measures:` by ID.

| ID | Instrument | Type | Measures | Used by |
|---|---|---|---|---|
| `veq` | [Virtual Embodiment Questionnaire](veq.md) | Validated (external) | ownership, agency, body change | most 0xx, 101, 201 |
| `ssq` | [Simulator Sickness Questionnaire](ssq.md) | Validated (external) | nausea, oculomotor, disorientation | locomotion and long sessions |
| `ipq` | [igroup Presence Questionnaire](ipq.md) | Validated (external) | spatial presence, involvement, realism | 008, 201 |
| `intent-control` | [Intent control ratings](intent-control.md) | Custom | perceived control, latency, effort, automaticity | 003–007, 010–013 |
| `mana-sensation` | [Mana sensation report](mana-sensation.md) | Custom | perceived movement, start/end location, strength, ownership of sensation | 101–107 |
| `casting-experience` | [Casting experience ratings](casting-experience.md) | Custom | agency, presence, power, effort, enjoyment, origin-in-body | 103, 105–107, 201 |

## Response file format

Every instrument writes rows to `responses.csv` in the session directory:

```text
session_id,block,condition_id,item_id,response
```

- `block`: block index from the randomized order in `session.yaml`.
- `condition_id`: condition ID from `conditions.yaml`. Fill it in **after** the block from the session log, not from memory, so blinding is preserved while you answer.
- `item_id`: item code (e.g. `VEQ_OWN_1`, `MS3`, `IC2`).
- `response`: number, or free text for open items (quote it if it contains commas).

## Licensing

Don't copy the item text of validated instruments into this repo. Get each one from its official source (linked on its page) and keep the local copy outside git if its licence requires that. The custom instruments here are this project's own and are written out in full.

## Administration rules

- Fill in questionnaires **immediately** after each block, while still reclined. Take the headset off only if the instrument is administered outside VR.
- Answer before looking at which condition ran.
- Use the same instrument version for the whole of an experiment.
