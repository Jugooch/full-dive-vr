# 107 — Silent-casting proficiency

- **Status:** planned
- **Version / Phase:** V5 (skill acquisition / silent casting)
- **Branch:** Synthetic Physiology
- **Depends on:** 106
- **Research source:** [07-version-roadmap](../../docs/research/07-version-roadmap.md) (V5), [06-incantations-and-casting](../../docs/research/06-incantations-and-casting.md) (progression)

## Question

Can practice reduce reliance on haptic guidance, physical effort and chants, so the user has actually **learned the magic system** rather than unlocked abilities?

## Hypothesis

Over weeks, the proficiency score S = f(effort⁻¹, accuracy, agency, cue-dependence⁻¹) rises. Shortened chants, and then silent casting, reach the success rate that full chants had earlier. Progression is unlocked by **demonstrated competency**, not XP.

## Hardware

- 106 setup

## Software

- Cue-level controller: full chant + strong haptics + obvious visuals → shortened chant → silent, with minimal guidance
- Competency tracker: stability, accuracy, required EMG amplitude, casting latency, dependence on verbal cues
- Design rule from the research: chanting stays useful even when optional (more efficiency, stability and maximum controllable mana)

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| FULL | Full chant, full guidance | chant: structured_full, mana_haptics: full, visual_guidance: high |
| SHORT | Shortened chant | chant: structured_short, mana_haptics: full, visual_guidance: medium |
| SILENT | Silent | chant: none, mana_haptics: reduced, visual_guidance: minimal |

## Procedure

1. Weekly probe: 10 casts per cue level, randomized (blinded on haptic level where possible).
2. Between probes, normal practice at the user's current competency tier.

## Measures

- S and its components; cue dependence = success(FULL) − success(SILENT)
- Required EMG amplitude over time (links to R/E in [metrics](../../docs/metrics.md))
- [casting-experience](../instruments/casting-experience.md), [mana-sensation](../instruments/mana-sensation.md)

## Success criteria / gate

- Silent-casting success approaches the earlier full-chant success at lower EMG amplitude. Opens 201.

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Learning curves per cue level. S over weeks.

## Notes / open questions

- The weights in S are undefined. Fix them before the first probe and record them in docs/metrics.md.
