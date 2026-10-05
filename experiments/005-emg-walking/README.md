# 005 — EMG walking

- **Status:** planned
- **Version / Phase:** V1 · Phase 3A
- **Branch:** Embodied Control
- **Depends on:** 004
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 3, Experiment 3A), [02-pod-concept](../../docs/research/02-pod-concept.md) (walking)

## Question

Can very small alternating left/right leg contractions, made while reclined, drive believable avatar walking with almost no physical movement?

## Hypothesis

Step cadence detected from alternating leg EMG maps to walking speed (v = k·f_step-intent) well enough for controlled walking. The avatar's animation handles the actual biomechanics.

## Hardware

- Reclining chair; 2 or more MyoWare channels on leg muscles (placement per manufacturer guidance); ESP32
- Optional: an IMU on each leg to measure how much the leg physically moves

## Software

- Decoder: L/R walking-pulse detection → cadence estimator → `IntentFrame.walk_forward` (scaled speed) + `confidence`
- Unreal: avatar locomotion driven only by `walk_forward`, with procedural walking animation
- **Raw biosignals never reach gameplay logic.** Only `IntentFrame` does.

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Cadence → speed | locomotion: emg, speed_gain_k: 0.7 m per step |
| B | Cadence → fixed speed | locomotion: emg, speed_mode: binary |

## Procedure

1. Recline and calibrate (rest, then alternating L/R pulses).
2. Free walking in an open area for 3 minutes per condition.
3. A short walk-and-stop task: walk to a marker and stop inside a circle, 10 trials.
4. Fill in the SSQ and intent-control.

## Measures

- Stop accuracy (distance from the circle centre), unintended starts/min, speed tracking
- Leg IMU displacement (if fitted)
- [intent-control](../instruments/intent-control.md), [SSQ](../instruments/ssq.md), [VEQ](../instruments/veq.md) agency

## Success criteria / gate

- Can walk and stop on purpose with < 1 unintended start/min
- No more than mild SSQ symptoms
- Opens the comparison in 006

## Safety

See [safety](../../docs/safety.md). Support, never restraint. Stop the session if you get motion sickness.

## Analysis plan

Per-condition stop error and false starts. Compare A and B on agency and control.

## Notes / open questions

- Turning isn't covered yet. Candidates are gaze/head yaw or asymmetric L/R activation. Add it in 006's course or a follow-up.
