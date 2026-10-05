# 008 — Sensory ablation

- **Status:** planned
- **Version / Phase:** V1 · Phase 4A (sensory recliner v1)
- **Branch:** Embodied Control
- **Depends on:** 002, 005
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 4, Experiment 4A), [03-diy-research-strategy](../../docs/research/03-diy-research-strategy.md) (physical feedback)

## Question

Which physical feedback systems actually add immersion? The pod shouldn't end up as "$15,000 of hardware whose effects your brain barely notices."

## Hypothesis

Each modality added in order (localized haptics → chair bass shaker → airflow) increases VEQ and IPQ scores, with diminishing returns. The ablation shows which modality gives the most per dollar.

## Hardware

Sensory recliner v1, with modalities added in this order:
1. Localized vibration: 8 zones (left/right shoulder, forearm, torso, thigh)
2. Dayton BST-1 bass shaker + amp, mounted structurally to the chair
3. Directional fans: front, left, right, possibly overhead, under microcontroller control
(Optional comparison: a commercial bHaptics vest)

## Software

- Haptic Bus routing for zones, bass-shaker audio events and fan speed (avatar velocity → airflow, wind direction → fan)
- Unreal scenario: walk through a windy forest → pick up a sword → strike a shield → explosion in the distance. Identical across conditions.

## Conditions

| ID | Label | Modalities |
|---|---|---|
| A | VR + audio | — |
| B | + localized haptics | vibration |
| C | + chair vibration | vibration, bass_shaker |
| D | + airflow | vibration, bass_shaker, airflow |

## Procedure

1. Same scenario per block (about 4 minutes). Randomized order.
2. Fill in the VEQ and IPQ after each block.
3. 3 or more sessions.

## Measures

- [VEQ](../instruments/veq.md), [IPQ](../instruments/ipq.md), [SSQ](../instruments/ssq.md)

## Success criteria / gate

- A ranking of modalities by their contribution to embodiment and presence. Use it to decide which modalities move into the pod design.

## Safety

See [safety](../../docs/safety.md). **No thermal modalities** (burn or cold-injury failure modes). Mount the bass shaker securely. Fans need guards.

## Analysis plan

Within-subject comparison A→D. Plot the cumulative score per added modality.

## Notes / open questions

- A pressure actuator (e.g. shoulder grab) could be a later condition E.
