# Roadmap

> **Central question:** How little physical movement and conventional input can a person use while still
> experiencing strong agency, ownership and presence in a freely moving virtual body, and can that
> body be given abilities the physical body doesn't have?

Source: [research/07-version-roadmap.md](research/07-version-roadmap.md) (final structure) and
[research/04-research-program-guide.md](research/04-research-program-guide.md) (Phases 0–6 detail).

## One platform, two branches

```text
                    PARTIAL-DIVE PLATFORM
                            │
             biosignals → Intent Bus → Unreal
                            │
                 synchronized haptics/logging
                            │
              ┌─────────────┴─────────────┐
              │                           │
      EMBODIED CONTROL (0xx)       SYNTHETIC PHYSIOLOGY (1xx)
       walking / hands                mana core
       locomotion                     mana routing
       virtual body                   spell formation
       motor intent                   chanting
              │                           │
              └─────────────┬─────────────┘
                            │
             FULL EMBODIED EXPERIENCE (2xx)
```

Synthetic physiology does **not** wait for locomotion to be "finished". Start thinking about V2 while
building V0/V1. Almost every purchase and line of code is shared.

## Versions

| Version | Question | Experiments | Gate to call it done |
|---|---|---|---|
| **V0** | Can subtle biological intent reliably control VR? | [000](../experiments/000-loop-latency/), [001](../experiments/001-baseline-embodiment/), [002](../experiments/002-synchronized-touch/), [003](../experiments/003-emg-binary-intent/) | Forearm flex → virtual hand closes → haptic response, reclined. 003 targets met (>95% rest/flex, <1 false activation/min, <150 ms, <30 s recalibration, survives electrode reattachment). 002 shows synchronous+congruent > delayed/mismatched ownership in *you*. |
| **V1** | Can I inhabit and locomote with minimal physical movement? | [004](../experiments/004-emg-multi-intent/), [005](../experiments/005-emg-walking/), [006](../experiments/006-emg-vs-joystick-locomotion/), [007](../experiments/007-minimal-activation/), [008](../experiments/008-sensory-ablation/), [009](../experiments/009-sword-embodiment/) | Lying in the chair, you can walk, stop, turn, grab and interact without controllers as the primary interface. R (physical/virtual activation) falling while agency stays high. |
| **V2** | Can I create a convincing fictional internal "mana core"? | [101](../experiments/101-mana-core-loop/), [102](../experiments/102-tactile-flow-illusion/), [103](../experiments/103-embodied-vs-arbitrary-casting/) | Sense → Gather → Route → Release feels like *something originated inside my body and traveled into my hand*. One phenomenon only: no elements, damage, enemies or skill tree. |
| **V2.5** | Can users learn to route that synthetic sensation? | [104](../experiments/104-learned-mana-sensation/) | Over weeks of training, blinded reduced/visual-only casts still produce reported movement (learned synthetic sensation), or a clear documented null result. |
| **V3** | Can multiple learned bodily patterns represent different magic? | [105](../experiments/105-mana-control-language/) | GATHER/ROUTE/SHAPE compose; fire / water / neutral are reliably distinguishable *internal control patterns*. |
| **V4** | Can chants improve control and learning? | [106](../experiments/106-incantations/) | Structured chant vs. arbitrary phrase vs. none compared on learning and control. |
| **V5** | Can practice reduce reliance on haptics, movement and chants? | [107](../experiments/107-silent-casting-proficiency/) | Proficiency S rises: less effort, less cue dependence, same accuracy and agency. Silent casting is *learned*, not unlocked. |
| **V6** | Can locomotion + embodiment + synthetic physiology coexist naturally? | [201](../experiments/201-locomotion-plus-magic/) | "I'm in this body, walking through this environment, and this body has abilities my physical body doesn't have." |

## Hardware stages (from the research guide's Phases)

The research guide's Phases describe the *build order*; versions describe the *questions*. They map:

| Phase | Build | Serves |
|---|---|---|
| 0 | Research platform: Unreal/OpenXR lab, LSL, logging, experiment runner, this repo | everything |
| 1 | Full-body avatar lab + 4-zone vibration haptics | V0 |
| 2 | One-channel → multi-channel EMG | V0 → V1 |
| 3 | Motionless locomotion | V1 |
| 4 | **Sensory recliner v1**: 8 zones, bass shaker, fans (support, not restraint) | V1, V2+ |
| 5 | EEG: execution first, imagery second | EEG track |
| 6 | EEG + EMG + gaze + context fusion | EEG track → any version |

## The EEG track

EEG is bought **only once Phases 1–4 produce useful results**. It never blocks a version. Its job is to
upgrade the *intent source* of whatever version is current. Because everything downstream consumes
`IntentFrame`s, the swap needs no rewrite.

| Exp | Question |
|---|---|
| [010](../experiments/010-eeg-motor-execution/) | Can I see reproducible sensorimotor changes at C3/C4/Cz during *real* movement? |
| [011](../experiments/011-eeg-motor-imagery/) | Two-class left/right imagery repeatably above chance on held-out runs and days (~70%+ is interesting) |
| [012](../experiments/012-eeg-in-vr/) | Forgiving real-time use (portal highlight / hand glow); offline vs. online accuracy gap |
| [013](../experiments/013-multimodal-intent-fusion/) | P(intent \| EEG, EMG, gaze, context): fusion beats any single signal |

## The 10-step sequence (from the research)

1. Create the Unreal/OpenXR experiment lab (avatar, mirror, interactions, VEQ logging, LSL, runner).
2. Build four-zone haptics; show synchronized touch > delayed/mismatched ownership.
3. Add one-channel EMG; reliably control one virtual hand action without buttons.
4. Expand EMG into locomotion while reclined and barely moving.
5. Quantify minimum required movement (train on progressively weaker contractions).
6. Build sensory recliner v1 (zones, bass transducer, airflow).
7. Run modality-ablation experiments.
8. Build a small interaction scenario (sword pickup/combat).
9. Only then buy passive EEG; execution first, imagery second.
10. Fuse EEG + EMG + VR context.

Synthetic physiology (V2+) starts in parallel from around step 3–4, reusing the EMG + haptics stack.

## Status

Update this table as work progresses; details go in [research-log.md](research-log.md).

| Version | Status | Since |
|---|---|---|
| Phase 0 platform | 🟡 repo scaffolded | 2026-10-05 |
| V0 | ⚪ not started | |
| V1 | ⚪ not started | |
| V2 | ⚪ not started | |
| V2.5–V6 | ⚪ not started | |
| EEG track | ⚪ deferred by design | |
