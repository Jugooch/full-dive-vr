# 007 — Minimal activation

- **Status:** planned
- **Version / Phase:** V1 · Phase 3C
- **Branch:** Embodied Control
- **Depends on:** 006
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Experiment 3C; "the sequence I would actually follow" #5)

## Question

How close to zero can muscle activation go while control accuracy, the false-positive rate and agency all stay high?

## Hypothesis

With progressive retraining on weaker contractions, the activation ratio R (EMG during virtual locomotion ÷ EMG during actual locomotion) can be pushed down substantially while agency stays near its 006 level. The research singles this out as possibly the most interesting thread in the project.

## Hardware

- Setup from 006. Also record reference EMG during **actual** walking or leg movement for the denominator of R.

## Software

- Decoder retraining at progressively lower activation targets. An on-screen target band (for calibration only) shows the intended contraction level.
- Logging of RMS EMG per step-intent

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Reference: normal contraction | activation_target: 1.0 |
| B | Reduced | activation_target: 0.5 |
| C | Subtle | activation_target: 0.25 |
| D | Minimal | activation_target: 0.1 |

Targets are fractions of the 005/006 calibration contraction. The steps are a procedural choice; adjust them once the data comes in.

## Procedure

1. Record reference EMG during actual leg movement (the denominator of R).
2. For each level, in **increasing difficulty** (not randomized, because this is a training progression): recalibrate at that level, run the 006 course 3 times, then rest 5 minutes for false activations.
3. Repeat over weeks. Track R at a fixed accuracy.

## Measures

- R = EMG_virtual / EMG_actual; E (physical-effort ratio) and the "partial-dive score" (E → 0 while agency → 1); see [metrics](../../docs/metrics.md)
- Accuracy, false activations/min, VEQ agency, [intent-control](../instruments/intent-control.md)

## Success criteria / gate

- A curve of R versus accuracy and agency over time. The gate for V1 is: reclined, you can walk, stop, turn, grab and interact without conventional controllers as the main input.

## Safety

See [safety](../../docs/safety.md). Sustained low-level contraction can cause fatigue, so take breaks.

## Analysis plan

Plot R against accuracy and against agency per session, and R over training weeks at a fixed accuracy threshold.

## Notes / open questions

- Is there a floor where EMG noise overwhelms intent? This is where EEG (010–013) becomes worth investigating.
