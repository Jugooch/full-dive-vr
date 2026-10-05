# 013 — Multimodal intent fusion

- **Status:** planned
- **Version / Phase:** EEG track · Phase 6
- **Branch:** Embodied Control
- **Depends on:** 012, 009
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 6), [03-diy-research-strategy](../../docs/research/03-diy-research-strategy.md) (combine EMG + EEG)

## Question

Does fusing EEG + EMG + gaze + scene context infer intent (e.g. "grab sword") more reliably and with less muscle activation than any single signal? You don't need telepathy, you need **sensor fusion**.

## Hypothesis

P(grab sword | EEG, EMG, gaze, context) is more confident and arrives earlier than EMG alone. This lets the EMG activation needed drop further (lower R/E) without losing accuracy.

## Hardware

- EEG (010–012), EMG (004), eye tracking, sensory recliner

## Software

- Fusion decoder: inputs X = [EEG, EMG, IMU, eye gaze, head orientation] plus a scene-context prior (what is interactable) → the same `IntentFrame`
- Unreal supplies the missing biomechanics (IK) once intent is confirmed

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | EMG only | sources: [emg] |
| B | EMG + gaze + context | sources: [emg, gaze, context] |
| C | EEG + EMG + gaze + context | sources: [eeg, emg, gaze, context] |
| D | EEG + gaze + context (no EMG) | sources: [eeg, gaze, context] |

## Procedure

1. Reuse the 009 sword scenario. 20 grab-and-strike trials per block, randomized, blinded on decoder source. The participant performs the same intent every time.
2. Repeat at reduced EMG activation targets (from 007).

## Measures

- Accuracy, false activations/min, intent-to-action latency, R/E effort ratios, VEQ agency ([metrics](../../docs/metrics.md))

## Success criteria / gate

- C beats A on accuracy or latency at an equal or lower activation target. This is the start of "replacing explicit muscular commands with inferred intent".

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Per-source performance at each activation level. Held-out days only for the fusion model training.

## Notes / open questions

- Calibrated probabilities matter for fusion. Check reliability diagrams.
