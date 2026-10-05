# Research protocol

How every experiment in this repo is run. The single-participant (n = 1, self-experimentation) design
is deliberate: the goal is to establish **your own** effects and variance, not to publish population
claims. The rules below exist so that "it felt better" can't sneak in as a result.

## 1. Pre-register lightly

Before the first session of an experiment, its `README.md` must have a hypothesis, conditions,
measures and a **gate** (what result advances the roadmap). Once an experiment is `active`, changes go
in *Notes / open questions* with a date. Material changes → a new experiment ID.

## 2. Randomize and blind programmatically

- `partialdive session init` generates the block order from `conditions.yaml`. **Never pick conditions by hand.**
- `randomize: true` balances conditions within rounds; `full` for unpredictable catch trials (104);
  `false` only for deliberate training progressions (007).
- When `blinded: true`, the participant-facing display shows only block codes. `sealed.yaml` holds the key;
  don't open it until `partialdive session unblind` after data collection.
- Some experiments can't be blinded (the input method or modality is perceptible: 006, 008, 103). Say so in the
  protocol, and lean harder on objective measures there.

## 3. Establish baseline variance first

Repeat baselines (001) on several different days before interpreting any difference. An effect smaller
than your day-to-day variation is not an effect.

## 4. Never leak between train and test

- **Never randomly split adjacent windows** of a biosignal recording into train/test; neighboring windows
  are strongly correlated and accuracy will be massively inflated.
- Split by **run** (runs 1–3 train → run 4 validate), then by **day** (Monday train → Tuesday held-out).
- Don't keep watching accuracy and unconsciously change the experiment mid-run.

## 5. Log everything automatically

Each session directory holds `session.yaml` (repo commit, dirty flag, hardware profile, firmware
versions, block codes, metrics, questionnaire scores, notes), `calibration.yaml`, `responses.csv`, and
the XDF (not committed). If it isn't recorded, it didn't happen: six months later "version 14 felt
really good" is useless without the exact code, hardware and condition.

## 6. Questionnaires immediately

Administer post-block questionnaires immediately after the block, ideally in-headset so the
experience isn't broken by taking the headset off. Use the validated instruments (VEQ, IPQ, SSQ) as
published. Don't paraphrase items.

## 7. Write it up, including failures

Every session gets an entry in [`research-log.md`](research-log.md). When an experiment's gate is
evaluated, write the result there and update its `Status` and the [roadmap](roadmap.md). Null results
are results. "Fix this before proceeding" (e.g. no ownership difference in 002) is a valid outcome.

## 8. Comfort and stopping rules

Stop a session on discomfort, nausea (rising SSQ), fatigue or any skin irritation, and log why. See [safety.md](safety.md).

## 9. Analysis

- Load XDF with `pyxdf`; align on LSL timestamps; event markers from `pdive.game`, `pdive.experiment`.
- Compute metrics with `partialdive.analysis` so definitions stay consistent across experiments.
- Notebooks in `analysis/notebooks/<experiment-id>/`, figures and summaries in `analysis/reports/`.
- Report effect sizes with your own baseline variance alongside, not just means.
