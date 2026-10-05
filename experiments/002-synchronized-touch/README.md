# 002 — Synchronized touch

- **Status:** planned
- **Version / Phase:** V0 · Phase 1B
- **Branch:** Embodied Control
- **Depends on:** 000, 001
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 1B), [03-diy-research-strategy](../../docs/research/03-diy-research-strategy.md) (body ownership)

## Question

Does physical touch that is synchronized and in the right place make ownership of the virtual body stronger than delayed, misplaced or absent touch?

## Hypothesis

Synchronous and spatially congruent > delayed or mismatched > none, measured as VEQ ownership. Delays over about 300 ms should clearly weaken ownership, consistent with the rubber-hand delay literature ([S56](https://journals.plos.org/plosone/article?id=10.1371%2Fjournal.pone.0006185)).

## Hardware

- Setup from 001
- ESP32 + 4× DRV2605L + 4 vibration motors (ERM/LRA): left forearm, right forearm, chest, shoulder

## Software

- Unreal: virtual objects touch the right forearm. Collision → Haptic Bus `HAPTIC_RIGHT_FOREARM strength=0.6 duration=120ms`
- Haptic Bus applies the per-condition delay and zone remap. Unreal never knows which condition is running.
- `partialdive session init` generates the blinded order

## Conditions

| ID | VR contact | Physical feedback | Key parameters |
|---|---|---|---|
| A | right arm | immediate, right arm | delay 0, no remap |
| B | right arm | ~150 ms delayed, right arm | delay 150 |
| C | right arm | > 300 ms delayed, right arm | delay 350 |
| D | right arm | immediate, **left** arm | remap right_forearm → left_forearm |
| E | right arm | none | haptics off |

## Procedure

1. Attach the actuators. Confirm each zone fires by sending a test pulse (not inside VR).
2. Per block: about 2 minutes of virtual objects touching the right forearm at irregular intervals while you watch your arm.
3. Fill in the VEQ after each block.
4. Order is randomized and blinded. **Don't** check which condition ran until all the questionnaires are done.
5. Run 3 repetitions per condition over at least 2 sessions.

## Measures

- [VEQ](../instruments/veq.md), with ownership as the main outcome
- Logged true actuator onset versus collision time (check the delay was really applied)

## Success criteria / gate

- Ownership A > B, C, D, E in my own data, beyond the day-to-day SD from 001.
- **If there's no difference at all, fix this before going on** (avatar, timing, actuator placement). The research treats this as a hard gate.

## Safety

See [safety](../../docs/safety.md). Vibration motors only. Check for skin irritation at the actuator sites.

## Analysis plan

Per-condition mean VEQ ownership and agency with SD. Paired comparison against A. Plot ownership against delay for A/B/C.

## Notes / open questions

- Is the useful delay threshold sharper than 150 vs 300 ms? Later sweep: 0/50/100/200/300 ms.
- The effect of an incongruent location (D) compared with delay is interesting for the "sensory codec" question.
