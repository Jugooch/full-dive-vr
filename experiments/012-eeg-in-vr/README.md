# 012 — EEG in VR

- **Status:** planned
- **Version / Phase:** EEG track · Phase 5C
- **Branch:** Embodied Control
- **Depends on:** 011
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Experiment 5C)

## Question

How different is offline classifier accuracy from usable real-time BCI control in VR?

## Hypothesis

Online performance is noticeably below offline accuracy. A forgiving interaction (confidence above a threshold for N ms → confirm) makes it usable anyway.

## Hardware

- Setup from 011 plus the VR headset. Watch for headset-strap pressure or artifacts on the electrodes.

## Software

- Online decoder → `IntentFrame` (same struct as EMG)
- Unreal: forgiving tasks only. **Not walking first.** Left imagery → left portal highlighted / left virtual hand glows; right → right.
- Dwell confirmation: confidence > threshold for N ms → confirm action

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Portal selection | task: portal_select, dwell_ms: 1000 |
| B | Hand glow | task: hand_glow, dwell_ms: 1000 |

## Procedure

1. Calibrate a fresh decoder (011 pipeline).
2. 20 cued selections per block. Randomized order.
3. Fill in intent-control after each block.

## Measures

- Online accuracy compared with the offline accuracy for the same session; time to selection; false selections/min
- [intent-control](../instruments/intent-control.md)

## Success criteria / gate

- Online control feels intentional (IC1 ≥ 5) with above-chance accuracy. Opens 013.

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Offline versus online accuracy gap per session. Effect of the dwell threshold.

## Notes / open questions

- Is it worth tuning dwell N per user? Sweep N later.
