# 101 — Mana core loop

- **Status:** planned
- **Version / Phase:** V2 (Synthetic physiology v0)
- **Branch:** Synthetic Physiology
- **Depends on:** 003, 002
- **Research source:** [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md) (Mana Experiment V0), [07-version-roadmap](../../docs/research/07-version-roadmap.md) (V2)

## Question

**Can I make this feel like something started inside my body and travelled into my hand?**

## Hypothesis

A fixed virtual "mana core" (abdomen), established by a subtle haptic pulse and a visual glow, combined with a **Sense → Gather → Route → Release** loop driven by abdominal and forearm EMG, produces a coherent experience of an internal resource moving outward.

Deliberately constrained: **one fictional bodily phenomenon.** No elements, damage system, enemies or skill tree.

## Hardware

- 1 abdominal EMG sensor (MyoWare, placement per manufacturer guidance) + 1 right-forearm EMG
- 2–4 vibration motors: abdomen/back (core), chest, shoulder, forearm/hand
- ESP32, VR headset
- Optional: respiration/stretch-belt sensor

## Software

- Decoders: core activation C ∈ [0,1], C = (EMG − μ_rest)/(μ_max − μ_rest); forearm routing intent; release gesture
- Mana state: dM/dt = kC (continuous, not binary)
- Unreal: avatar with an internal core glow, low spatial hum, particles, avatar breathing synced to real respiration if the sensor is present; generic mana ball released from the hand
- Haptic Bus: core pulse (Sense), intensity ∝ M (Gather), sequenced flow core → chest → shoulder → arm → hand (Route)

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Full loop | mana_haptics: full, visual_flow: true |

(The single-condition pilot establishes the loop. The comparisons are in 102–104.)

## Procedure

1. **Sense**: relax with attention on the abdomen. A subtle repeating core pulse establishes where the core is.
2. **Gather**: slight abdominal activation. The core brightens and the pulse strengthens.
3. **Route**: subtle right-forearm activation. Timed haptics travel abdomen → chest → shoulder → arm → hand.
4. **Release**: a small, deliberate hand or forearm signal. A generic mana ball leaves the hand.
5. 20 casts per session. Fill in the mana-sensation report after every 5 casts and casting-experience at the end.
6. **Comfortable normal breathing only.** No breath-holding or hyperventilation as part of the mechanic.

## Measures

- [mana-sensation](../instruments/mana-sensation.md), [casting-experience](../instruments/casting-experience.md), [VEQ](../instruments/veq.md)
- Cast success rate; core activation level; time per stage

## Success criteria / gate

- MS1 "felt something move" = yes on most casts, with MS2 = core and MS3 = hand
- The loop is reliable enough (cast success ≥ 80 %, a procedural choice) to run 103 and 104

## Safety

See [safety](../../docs/safety.md). Abdominal contraction must stay light. No breath-holding or hyperventilation. Stop if dizzy.

## Analysis plan

Descriptive at first: the distribution of start/end locations and strength. Free-text themes.

## Notes / open questions

- Back versus front placement of the core actuator?
- Does syncing the avatar's breathing (embreathment, [S70](https://pmc.ncbi.nlm.nih.gov/articles/PMC6985859/)) strengthen the core? A candidate follow-up condition.
