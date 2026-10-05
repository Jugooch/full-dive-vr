# 0006 — Experiment conditions applied in the haptic bus, blinded

- Status: accepted
- Date: 2026-10-05

## Context
Key experiments manipulate physical feedback relative to the virtual event: delay (0 / ~150 / >300 ms),
wrong location, none (002); modality ablation (008); actuator subsets (102); reduced or visual-only
mana (104). The research insists conditions are randomized programmatically and hidden from the
participant.

## Decision
Unreal always emits the true `HapticEvent`. The Python haptic bus applies `HapticCondition` built from
the block params (`block/1` message or `--session-dir/--block`). Session tooling creates blinded block
codes with a sealed key (`sealed.yaml`) and hashed seed.

## Consequences
One code path for all manipulations, all logged to LSL with dispatch lag. Unreal still receives params
for visual manipulations (mirror, visual flow, locomotion source) and must not display them.
