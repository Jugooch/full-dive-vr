# 004 — EMG multi-intent classification

- **Status:** planned
- **Version / Phase:** V1 · Phase 2B
- **Branch:** Embodied Control
- **Depends on:** 003
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Experiment 2B)

## Question

Can 4 EMG channels be classified into several separate intents (rest, left-hand action, right-hand action, left walking pulse, right walking pulse) reliably enough to drive an avatar?

## Hypothesis

Simple time-domain features with linear classifiers reach usable accuracy, and generalize across runs and days better than complex models trained on the same small dataset.

## Hardware

- 4× MyoWare 2.0 (A: left forearm, B: right forearm, C: left leg, D: right leg; placements per manufacturer guidance), battery shields, ESP32

## Software

- `partialdive` feature extraction: MAV, RMS, variance, waveform length, envelope peak
- Classifiers: logistic regression, LDA, random forest (scikit-learn). No deep learning yet.
- Decoder output → `IntentFrame` probabilities and `confidence`

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Logistic regression | classifier: logreg |
| B | LDA | classifier: lda |
| C | Random forest | classifier: random_forest |

The conditions are evaluated offline on the same recordings, then the best model runs online.

## Procedure

1. Cued recording: 5 classes × at least 30 trials per run, 4 runs per day, on 2 or more days.
2. Train on runs 1–3 and validate on run 4. Then train on day 1 and evaluate on day 2.
3. Run the best model online in VR for a 5-minute free-use block, plus a 5-minute rest block for false activations.

## Measures

- Per-class precision, recall and F1; confusion matrix; false activations/min; missed intents; latency
- [intent-control](../instruments/intent-control.md)

## Success criteria / gate

- Held-out-day macro F1 good enough that the online free-use block feels intentional, with < 1 false activation/min at rest
- Walking pulses L/R separable enough to feed 005

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

**Never randomly split adjacent windows.** Split by run and by day only. Report held-out results separately from same-session results.

## Notes / open questions

- How many channels are actually needed? Try ablating channels one at a time.
