# 009 — Sword embodiment

- **Status:** planned
- **Version / Phase:** V1 · Phase 4B
- **Branch:** Embodied Control
- **Depends on:** 004, 008
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Experiment 4B), [02-pod-concept](../../docs/research/02-pod-concept.md) (combat)

## Question

Can a sword interaction done entirely through subtle intent, with sensory feedback that all agrees, convince the brain the virtual action happened, even though the sword's real weight isn't reproduced?

## Hypothesis

Gaze plus EMG grab intent plus EMG arm intent, with the game predicting the swing, makes a strike that feels like **my** strike when the impact feedback (forearm haptic + bass shaker + spatial audio) is synchronized.

## Hardware

- Forearm/arm EMG (from 004), sensory recliner v1 (from 008), eye tracking if the headset supports it

## Software

- Sequence: look at sword → EMG grab intent → virtual hand grabs → EMG arm intent → game predicts the swing (IK / animation) → sword hits target → forearm haptic + bass-shaker impact + spatial audio
- Context check: only grabbable objects can be grabbed

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Intent + full impact feedback | input: emg, impact_feedback: [haptic, bass_shaker, audio] |
| B | Intent + audio only | input: emg, impact_feedback: [audio] |
| C | Controller + full feedback | input: controllers, impact_feedback: [haptic, bass_shaker, audio] |

## Procedure

1. 20 grab-and-strike trials per block. Randomized order.
2. Fill in the VEQ and intent-control after each block.

## Measures

- Strike success rate, false grabs, intent → strike latency
- [VEQ](../instruments/veq.md), [intent-control](../instruments/intent-control.md)

## Success criteria / gate

- A ownership/agency ≥ C, and A > B. This is the major V1 milestone interaction.

## Safety

See [safety](../../docs/safety.md). No physical swinging is required. If you swing involuntarily, check that nothing within reach can be hit.

## Analysis plan

Per-condition embodiment and performance. Note subjective "weight" reports in free text.

## Notes / open questions

- Can the brain learn that "this sensory pattern means this weapon weighs this much"? Later variant: different haptic signatures per weapon.
