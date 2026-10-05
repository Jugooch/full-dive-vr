# 201 — Locomotion + magic

- **Status:** planned
- **Version / Phase:** V6 (merge)
- **Branch:** Integration
- **Depends on:** 007, 107
- **Research source:** [07-version-roadmap](../../docs/research/07-version-roadmap.md) (V6)

## Question

Can locomotion, embodiment and synthetic physiology work together naturally? Does the experience shift from "I'm lying in a chair controlling a VR character" to "**I'm in this body, walking through this environment, and this body has abilities my physical body doesn't have**"?

## Hypothesis

Combining EMG locomotion, subtle-intent interaction, the mana core and chanting while moving keeps embodiment at the level of the separate branches, or raises it, without the two control vocabularies interfering.

## Hardware

- Full sensory recliner + leg, forearm and abdominal EMG + microphone (+ EEG if 013 succeeded)

## Software

- Integrated scene: walk through an environment and cast while moving
- An intent arbitration rule for EMG channel conflicts (e.g. a core brace during walking). This is a procedural choice to design here.

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Locomotion only | locomotion: emg, magic: false |
| B | Magic only (stationary) | locomotion: none, magic: true |
| C | Integrated | locomotion: emg, magic: true |

## Procedure

1. Scenario of about 5 minutes per block. Randomized. 3 or more sessions.
2. VEQ, IPQ, SSQ, casting-experience after each block.

## Measures

- [VEQ](../instruments/veq.md), [IPQ](../instruments/ipq.md), [SSQ](../instruments/ssq.md), [casting-experience](../instruments/casting-experience.md)
- Cross-interference: false casts while walking, false steps while casting

## Success criteria / gate

- C embodiment ≥ max(A, B) with acceptable cross-interference. This is the project's integration milestone.

## Safety

See [safety](../../docs/safety.md). Longer sessions mean you should watch fatigue and cybersickness.

## Analysis plan

Within-subject comparison. Interference rates per minute.

## Notes / open questions

- Free-text reports of the "this body has abilities" shift are the qualitative headline result.
