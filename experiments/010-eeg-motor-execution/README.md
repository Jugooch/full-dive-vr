# 010 — EEG motor execution

- **Status:** planned
- **Version / Phase:** EEG track · Phase 5A (can upgrade any version's intent source)
- **Branch:** Embodied Control
- **Depends on:** 007 (soft: buy EEG only once Phases 1–4 give useful results)
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 5, Experiment 5A)

## Question

Can I see reproducible sensorimotor EEG changes over C3/C4 (and Cz) during **actual** hand movement? This validates the whole EEG chain before trying imagery.

## Hypothesis

Real right-hand and left-hand movement produce reproducible μ (≈ 8–15 Hz) and β (≈ 16–31 Hz) changes, strongest over the opposite hemisphere (C3 for the right hand, C4 for the left).

## Hardware

- OpenBCI Ganglion (4 ch, 200 Hz, BLE) is the suggested first board. Alternatives: Cyton (8 ch) or Neurosity Crown (includes C3/C4/CP3/CP4). **Not** Muse 2: its sensors aren't over motor cortex.
- Electrodes at C3, C4, Cz (+ nearby)
- **Battery-only** power while connected to the body

## Software

- BrainFlow acquisition → LSL; MNE-based preprocessing (60 Hz notch, band extraction, epoching around cues)

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Right-hand movement | task: move_right_hand |
| B | Left-hand movement | task: move_left_hand |
| R | Rest | task: rest |

## Procedure

1. Check electrode contact and impedance. Log it.
2. Cued trials: REST → move right hand → REST → move left hand. 30 or more trials per class, 3 or more runs.
3. No VR yet. Use a simple cue display.

## Measures

- μ/β band power change (ERD/ERS) per channel per class; signal quality: SNR, drift, impedance ([metrics](../../docs/metrics.md))

## Success criteria / gate

Reproducible lateralized sensorimotor changes across runs. This validates electrodes, placement, contact, acquisition, timestamps and preprocessing. **Only then move on to imagery (011).**

## Safety

See [safety](../../docs/safety.md). EEG **recording only**. Battery isolation from mains while connected. Hobby EEG is not a certified medical device.

## Analysis plan

Time–frequency plots per channel and class, averaged per run. Check consistency across runs.

## Notes / open questions

- Is 4 channels enough? The research says yes for first exploration.
