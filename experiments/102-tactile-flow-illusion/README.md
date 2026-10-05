# 102 — Tactile flow illusion

- **Status:** planned
- **Version / Phase:** V2
- **Branch:** Synthetic Physiology
- **Depends on:** 002
- **Research source:** [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md) (draw the mana out)

## Question

What is the fewest actuators that, with visual mana flow, still produce the perception that **something moved from my core through my arm**?

## Hypothesis

Timed sequential vibration produces illusory motion between actuators, and vision strongly dominates tactile-motion illusions ([S73](https://pmc.ncbi.nlm.nih.gov/articles/PMC11958647/)). So 2 actuators (core + arm) with visual flow can approach the perceived continuity of 6 actuators.

## Hardware

- Up to 6 vibration motors: abdomen (core), upper torso, shoulder, upper arm, forearm, hand (right side). ESP32 + DRV2605L.

## Software

- Haptic Bus sequencer: per-actuator onset offset and duration (staggered, not simultaneous)
- Unreal: visual mana flow from core to hand, toggleable

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | 6 actuators + visual | actuators: 6, visual_flow: true |
| B | 6 actuators, no visual | actuators: 6, visual_flow: false |
| C | 3 actuators + visual | actuators: [core, shoulder, hand], visual_flow: true |
| D | 2 actuators + visual | actuators: [core, forearm], visual_flow: true |
| E | 2 actuators, no visual | actuators: [core, forearm], visual_flow: false |
| F | Visual only | actuators: 0, visual_flow: true |

## Procedure

1. Passive presentation (no EMG needed): 10 flows per block. Randomized and blinded.
2. Fill in the mana-sensation report after each block (MS1–MS6, MS9).

## Measures

- [mana-sensation](../instruments/mana-sensation.md): perceived movement, start/end, continuity (MS6), body-belonging (MS5)

## Success criteria / gate

- The smallest actuator set whose continuity and movement scores aren't meaningfully lower than A. That becomes the default routing hardware for 104 and later.

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Continuity and movement by actuator count × visual. Check the visual-dominance prediction (D ≈ A, E < D).

## Notes / open questions

- Sweep the stagger timing (inter-onset interval) once the actuator count is chosen.
