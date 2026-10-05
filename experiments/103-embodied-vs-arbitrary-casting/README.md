# 103 — Embodied vs arbitrary casting

- **Status:** planned
- **Version / Phase:** V2
- **Branch:** Synthetic Physiology
- **Depends on:** 101
- **Research source:** [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md) (the key experiments)

## Question

Which control method makes you **feel like you actually possess the fictional ability**? (Not "which is the fastest UI?")

## Hypothesis

Core gather → tactile flow through the arm → hand charge → release (C) is slower than a button (A) but scores highest on agency, perceived power and "originated from my body".

## Hardware

- 101 setup + controller

## Software

- Same fireball effect in all conditions. Only the input path changes.

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Button → fireball | cast_input: button |
| B | Hand flex → fireball | cast_input: hand_flex |
| C | Core gather → flow → charge → release | cast_input: core_flow, mana_haptics: full |

## Procedure

1. A short familiarization per condition.
2. 15 casts at targets per block. Randomized order. 3 or more sessions (learning time is a measure).
3. Casting-experience and mana-sensation after each block.

## Measures

- Agency, presence, perceived power, physical effort, learning time, enjoyment, how strongly the magic felt like it came from the body ([casting-experience](../instruments/casting-experience.md), [mana-sensation](../instruments/mana-sensation.md))
- Cast time and accuracy

## Success criteria / gate

- A clear answer on whether embodied control (C) beats arbitrary control (A/B) on CE7/CE8. A negative result is valid too and should shape V3.

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Within-subject comparison per item. Track learning curves (cast time per session) for C.

## Notes / open questions

- C is expected to be slower. That's fine.
