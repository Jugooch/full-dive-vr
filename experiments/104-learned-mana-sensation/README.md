# 104 — Learned mana sensation

- **Status:** planned
- **Version / Phase:** V2.1 / V2.5 (learnable routing)
- **Branch:** Synthetic Physiology
- **Depends on:** 101, 102
- **Research source:** [07-version-roadmap](../../docs/research/07-version-roadmap.md) (V2.1), [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md) (training could produce something close to "sensing mana")

## Question

Does repeated training turn core activation → expected internal sensation → routing → hand into **one learned sensorimotor sequence**? After training, do people report feeling mana move even when haptics are reduced or absent?

## Hypothesis

After weeks of training, blinded reduced-haptic and visual-only casts increasingly produce "felt it move" reports (MS1) that look like the body's own sensation (MS5). Phantom-touch evidence (89 % reported some sensation from seen but unfelt touch, [S74](https://pmc.ncbi.nlm.nih.gov/articles/PMC10507094/)) makes this plausible but not guaranteed.

## Hardware

- 101 setup, using the minimal actuator set from 102

## Software

- Training mode: full haptics always
- Probe mode: per-cast haptic condition chosen by the randomizer and hidden from the display

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| F | Full haptics | mana_haptics: full |
| R | Reduced haptics | mana_haptics: reduced (core only, 50 % amplitude) |
| V | Visual only | mana_haptics: none |

## Procedure

1. **Training phase**: about 15 minutes per day of full-haptic 101 casting, over several weeks (procedural choice: start at 4 weeks, at least 5 days/week).
2. **Probe sessions** (week 0 before training, then weekly): 30 casts with blinded per-cast conditions F/R/V (ratio 1:1:1). After each cast: did you feel something move? Where did it start? Where did it end? How strong? External vibration or part of your virtual body?
3. Don't look at the condition log until the probe session is finished.

## Measures

- [mana-sensation](../instruments/mana-sensation.md) per cast; EMG activation levels; cast success

## Success criteria / gate

- A trend over weeks in MS1/MS5 for R and V casts. Any reliable change is a result.
- Routing works reliably (V2.5): unblinded cast success ≥ 80 % with stable activation. This opens 105.

## Safety

See [safety](../../docs/safety.md). Keep daily training short. No breath-holding or hyperventilation.

## Analysis plan

Proportion of MS1 = yes per condition per week. Location accuracy (MS2/MS3 versus the designed route). Compare free text over time.

## Notes / open questions

- Expectation and demand effects: the participant is also the experimenter. Blinding per cast is the main safeguard, so keep it strict.
