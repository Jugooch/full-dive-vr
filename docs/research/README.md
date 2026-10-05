# Research Record

This folder is a faithful, structured record of the ChatGPT conversation **"Full Dive VR Research"** that this project grew out of.

- **Original:** <https://chatgpt.com/share/6ac3bf37-28ac-83e9-b610-043722cfba2f>
- **Archived:** 2026-10-05
- **Status:** reference record. Don't rewrite these files to reflect new decisions. Record changes in the operational docs and the [research log](../research-log.md).

The conversation is split by question. Each user question became one document, and the answer was reorganized into headed sections. All numbers, prices, equations, tables, diagrams and citations are kept. Citations appear as `[S#]` and resolve in [sources.md](sources.md).

## Reading order

| # | Document | What it covers |
|---|---|---|
| 01 | [Full-dive feasibility](01-full-dive-feasibility.md) | Subsystems a full-dive machine needs, why the brain is easier to read than to write, readiness as of 2026, motor isolation, why the user shouldn't be put to sleep, "partial dive", generations 1–4, timelines |
| 02 | [Pod concept](02-pod-concept.md) | Full-body pod that uses the real eyes and ears, decoding intent instead of joint angles, walking and combat in a pod, why a capsule simplifies the engineering |
| 03 | [DIY research strategy](03-diy-research-strategy.md) | How to do meaningful non-invasive research: EMG before EEG, motionless locomotion, body ownership, physical feedback, Intent Bus, sensor fusion, metrics, safety boundary, first five milestones |
| 04 | [Research program guide](04-research-program-guide.md) | Full staged guide: Phase 0 platform and software stack, Phases 1–6 with experiments 1A–5C, IntentFrame, session metadata, metrics, first purchases, 10-step sequence |
| 05 | [Synthetic physiology: mana core](05-synthetic-physiology-mana.md) | Synthetic interoception, the "mana organ", Sense → Gather → Draw → Release, tactile-flow illusions, continuous mana, Mana Experiment V0 |
| 06 | [Incantations and casting](06-incantations-and-casting.md) | Chants as a scaffold and a spell language, the casting sequence, progression to silent casting, voice tech, schools, compound magic, spell customization |
| 07 | [Version roadmap](07-version-roadmap.md) | Final structure: one platform with two branches (embodied control and synthetic physiology), versions V0–V6 |
| — | [Sources](sources.md) | All 80 cited references, grouped by topic, with where each is cited |
| — | [Transcript](transcript.md) | Full cleaned transcript |

## Record vs. operational docs

These files record what the research said. The working plan derived from them is in:

- [`docs/roadmap.md`](../roadmap.md): versions, phases and advancement gates
- [`docs/architecture.md`](../architecture.md): Intent Bus, IntentFrame, Haptic Bus, LSL, voice pipeline
- [`docs/hardware.md`](../hardware.md): bill of materials by phase
- [`docs/research-protocol.md`](../research-protocol.md): randomization, blinding, held-out evaluation, session metadata
- [`docs/metrics.md`](../metrics.md): metric definitions
- [`docs/safety.md`](../safety.md): hard boundaries
- [`experiments/`](../../experiments/): one folder per experiment

## Caveats

- **Prices and product facts are as of the conversation (2026).** Re-verify them before buying.
- Timelines and estimates are the original assistant's speculation, not established facts.
- Sources were gathered by the original assistant. Read the primary source before relying on a specific claim.
