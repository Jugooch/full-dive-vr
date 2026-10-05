# Metrics

Every metric the project tracks, with one definition each. Code: `services/src/partialdive/analysis/`.
Rule from the research: *don't make the goal "feels cool"; measure things.* The aim is to be able to write
"EMG locomotion increased embodiment by 23% relative to joystick while adding 74 ms median input latency",
not "this version felt better".

## 1. Control

| Metric | Definition | V0 target (engineering goal, not biology) |
|---|---|---|
| Accuracy / precision / recall / F1 | Per-sample or per-trial detection vs. cued ground truth (`classification_metrics`) | > 95% rest/flex |
| False activations / min | Detector onsets during instructed rest ÷ minutes of rest (`false_activations_per_min`) | < 1 / min |
| Missed intents | Cues with no detection within 1 s (`onset_latencies` → NaN) | — |
| Command latency | Cue (or EMG onset) → IntentFrame state change; report **median** and p95 | < 150 ms perceived |
| Loop latency | Virtual collision timestamp → actuator command dispatched (bus logs `lag_ms`) | ≈ 20 ms (exp. 000) |
| Information transfer rate | Wolpaw bits/min from N classes, accuracy, selection time (`wolpaw_itr`) | — |
| Calibration time | Time from electrodes on → decoder usable | < 30 s recalibration |
| Reattachment stability | Accuracy after removing and re-attaching electrodes vs. before | still meets targets |

## 2. Signal quality

SNR (flex envelope mean ÷ rest SD), baseline drift (rest mean change over a session), signal amplitude,
electrode-reattachment stability. EEG adds impedance/contact quality and line-noise power.

## 3. Immersion (questionnaires, see [`experiments/instruments/`](../experiments/instruments/))

| Construct | Instrument |
|---|---|
| Body ownership, agency, change in perceived body schema | **VEQ** (Virtual Embodiment Questionnaire) [S51, S52] |
| Presence: spatial presence, involvement, experienced realism | **IPQ** |
| Cybersickness | **SSQ** (Simulator Sickness Questionnaire) [S42] |
| Mana sensation (custom) | `mana-sensation` |
| Casting experience (custom) | `casting-experience` |
| Intent control (custom) | `intent-control` |

## 4. Performance

Task completion time, overshoot, error count, trajectory error, unintended movement (e.g. on the
006 course: start → 10 m → turn → obstacle → table → stop in circle).

## 5. Physical effort: the partial-dive metrics

**R** (experiment 007, locomotion):

$$R = \frac{\text{EMG activity during virtual locomotion}}{\text{EMG activity during actual locomotion}}$$

**E** (any task, project-wide):

$$E = \frac{\text{virtual-task muscle activation}}{\text{normal-task muscle activation}}$$

Long-term objective: $E \rightarrow 0$ while $\text{Agency} \rightarrow 1$. Both are computed with
`effort_ratio()` on mean envelope over matched task windows.

**Movement ratio** (V1 framing): physical movement required ÷ virtual movement produced → 0.

**Partial-dive score.** Report the pair **(E, VEQ agency)** together, never one alone. A run "improves"
only if E falls without agency (or ownership) falling, judged against your own baseline variance. Don't
collapse it into a single weighted number until there's enough data to justify weights (log the
decision as an ADR when you do).

## 6. Synthetic physiology

**Core activation**: $C = \dfrac{EMG - \mu_\text{rest}}{\mu_\text{max} - \mu_\text{rest}} \in [0,1]$,
mana accumulation $\dfrac{dM}{dt} = kC$.

**Draw intent**: $D_R = w_1E_\text{core} + w_2E_\text{right arm} + w_3R_\text{breathing} + w_4G_\text{hand}$.

**Cast quality**: $Q = w_mM + w_vV + w_cC + w_fF$ (mana supplied, chant accuracy, control stability,
form accuracy). Logged per cast in `mana-spell/1`; *felt*, never shown as HUD percentages.

**Cue dependence** (107): success rate with full cues − success rate silent, e.g.
`success(FULL) − success(SILENT)`. Falls as skill grows.

**Proficiency S** (V5):

$$S = f(\text{physical effort}^{-1}, \text{accuracy}, \text{agency}, \text{cue dependence}^{-1})$$

Components tracked separately: stability, accuracy, required EMG amplitude, casting latency,
dependence on verbal cues. **Fix the weights of f before the first 107 probe** and record them in an ADR.
Changing them afterwards invalidates comparisons.

## 7. EEG-specific

Two-class chance = 0.5. Advance when **repeatably above chance on held-out runs/sessions** (exact
binomial `above_chance_p`); sustained ~70%+ on genuinely held-out two-class trials is "interesting for a
hobbyist system". Always report the offline-vs-online gap (012).

## Dashboard

The five categories (control, signal quality, immersion, performance, physical effort) are the
dashboard sections. Build it in `analysis/` from the `session.yaml` metrics once there's data to plot.
