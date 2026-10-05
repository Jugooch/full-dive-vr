# 006 — EMG vs joystick locomotion

- **Status:** planned
- **Version / Phase:** V1 · Phase 3B
- **Branch:** Embodied Control
- **Depends on:** 005, 001
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Experiment 3B), [03-diy-research-strategy](../../docs/research/03-diy-research-strategy.md) (motionless locomotion A/B/C)

## Question

Does locomotion driven by body signals increase **agency** enough to make up for worse raw control compared with a joystick?

## Hypothesis

The joystick wins on completion time and precision. EMG locomotion scores higher on VEQ agency (and possibly ownership). **Don't expect EMG to beat the joystick on performance at first.**

## Hardware

- Setup from 005 plus a standard controller

## Software

- Unreal: a fixed course. Start → walk 10 m → turn left → walk around an obstacle → approach the table → stop inside the circle
- Course timing, trajectory logging and overshoot detection

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Joystick locomotion | locomotion: joystick |
| B | EMG locomotion | locomotion: emg |

## Procedure

1. A short practice run per condition (not scored).
2. Five scored course runs per block. Block order is randomized and alternated across sessions.
3. VEQ, SSQ and intent-control after each block.
4. At least 3 sessions on different days.

## Measures

| Metric | Why |
|---|---|
| Completion time | usability |
| Overshoot | control precision |
| Unintended movement | false activation |
| Latency | responsiveness |
| VEQ agency / ownership | embodiment |
| Sickness / discomfort (SSQ) | viability |

See [metrics](../../docs/metrics.md).

## Success criteria / gate

- The comparison is complete across 3 or more days. Either outcome is a result.
- A higher agency score for EMG is the interesting finding. Report it with the performance cost (e.g. "+X % agency at +Y ms median latency").

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Paired comparison per day. Report effect sizes with the 001 baseline SD as context.

## Notes / open questions

- Does the agency gap shrink or grow with practice? Track it across sessions.
