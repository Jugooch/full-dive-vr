# <id> — <Name>

- **Status:** planned
- **Version / Phase:** <V0–V6 / research-guide phase>
- **Branch:** <Embodied Control | Synthetic Physiology | Integration>
- **Depends on:** <experiment ids, or —>
- **Research source:** [<doc>](../../docs/research/<doc>.md)

## Question

<The single question this experiment answers.>

## Hypothesis

<The expected direction of the effect, stated so it can be shown wrong.>

## Hardware

<Sensors, actuators, headset, chair. Link to docs/hardware.md entries.>

## Software

<partialdive modules (acquisition, decoders, intent bus, haptic bus, session), Unreal scene pieces, firmware.>

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | | |

Mirror this table in `conditions.yaml`.

## Procedure

1. Setup and calibration
2. Blocks (randomized by `partialdive session init`)
3. Questionnaires after each block
4. Teardown and logging

## Measures

- Instruments: [`../instruments/`](../instruments/)
- Metrics: [`../../docs/metrics.md`](../../docs/metrics.md)

## Success criteria / gate

<What must be true before advancing to dependent experiments.>

## Safety

See [`../../docs/safety.md`](../../docs/safety.md). Experiment-specific notes:

## Analysis plan

<Comparisons, train/test split (held-out runs/days), plots.>

## Notes / open questions

