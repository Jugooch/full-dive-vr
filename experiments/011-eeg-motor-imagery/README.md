# 011 — EEG motor imagery

- **Status:** planned
- **Version / Phase:** EEG track · Phase 5B
- **Branch:** Embodied Control
- **Depends on:** 010
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Experiment 5B, basic signal pipeline)

## Question

Can left-hand and right-hand motor **imagery** be classified above chance on genuinely held-out runs and days?

## Hypothesis

A CSP + LDA baseline on μ/β-filtered epochs gets repeatably above chance (P_chance = 0.5) on held-out data. Something like 70 %+ sustained on held-out two-class trials would already be interesting for a hobbyist system.

## Hardware

- Setup from 010

## Software

- Pipeline: EEG → 60 Hz interference rejection → sensorimotor band extraction → epoch around cue → spatial features → CSP → LDA → left/right (MNE + scikit-learn)

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| L | Left-hand imagery | task: imagine_left_hand |
| R | Right-hand imagery | task: imagine_right_hand |

## Procedure

1. Trial: 2 s rest → visual cue → 3–4 s motor imagery → rest.
2. 20–40 clean trials per class to start. Several runs per session.
3. **Don't watch accuracy continuously** and unconsciously change the experiment.

## Measures

- Held-out accuracy, information transfer rate, calibration time ([metrics](../../docs/metrics.md))

## Success criteria / gate

Repeatably above chance on held-out runs and sessions, accurate enough to feel intentional. Target: about 70 %+ sustained on held-out two-class trials.

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

**Never randomly split adjacent EEG windows** (leakage). Runs 1–3 to train, run 4 to validate. Then Monday to train, Tuesday to evaluate. Report same-session and cross-day accuracy separately.

## Notes / open questions

- Foot imagery (Cz) as a third class later, possibly for walking.
