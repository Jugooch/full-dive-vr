# 0001 — Prove interaction with EMG before buying EEG

- Status: accepted
- Date: 2026-10-05

## Context
Non-invasive EEG has poor spatial resolution and SNR; motor imagery is noisy and calibration-heavy, and
useful hardware costs $625–3,000. Surface EMG gives a much cleaner "intended movement" signal for tens
of dollars per channel; Meta's Neural Band is built on exactly that principle [S30].
([research/03 §1](../research/03-diy-research-strategy.md), [research/04 Phase 2](../research/04-research-program-guide.md))

## Decision
V0–V5 use EMG (MyoWare 2.0) as the intent source. EEG is a separate track (Phases 5–6) started only
after Phases 1–4 produce useful results, beginning with *motor execution*, then imagery.

## Consequences
Cheap, fast iteration on the interaction model. "Intent" initially means subtle muscle activation, not
pure thought. The research question moves that boundary gradually (experiment 007, metric R/E).
Revisit if EMG can't reach the V0 targets.
