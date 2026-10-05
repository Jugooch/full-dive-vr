# 003 — EMG binary intent

- **Status:** planned
- **Version / Phase:** V0 · Phase 2A
- **Branch:** Embodied Control
- **Depends on:** 000
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 2, Experiment 2A), [03-diy-research-strategy](../../docs/research/03-diy-research-strategy.md) (EMG before EEG)

## Question

Can one EMG channel reliably separate "rest" from "flex" well enough to close and open a virtual hand without buttons?

## Hypothesis

The MyoWare **envelope** output on one large superficial muscle (forearm) can be separated with a simple threshold, T = μ_rest + kσ_rest. No machine learning is needed at this stage.

## Hardware

- MyoWare 2.0 + electrodes + MyoWare power shield (battery)
- ESP32 (serial/BLE)
- Electrodes placed by **manufacturer documentation** ([S58](https://learn.sparkfun.com/tutorials/getting-started-with-the-myoware-20-muscle-sensor-ecosystem/all))

## Software

- `partialdive` acquisition (envelope → LSL), calibration routine, threshold decoder → `IntentFrame.grab_right`
- Unreal: virtual hand closes when `grab_right` is active and opens on release

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Same-session test | k = 3, test immediately after calibration |
| B | Reattach test | remove and reattach the electrodes, recalibrate, then test |
| C | Rest-only false-activation run | 5 min relaxed in VR, no intended flexes |

## Procedure

1. Calibration: about 30 s rest, then 30–50 cued REST/FLEX repetitions. **Plot the signal** before doing anything else.
2. Compute μ_rest and σ_rest and set T. Choose k from the plot (start at k = 3).
3. A: 50 cued flex/rest trials in VR. Log detections.
4. C: 5 minutes of relaxed rest in VR. Count false activations.
5. B: take off the electrodes, put them back, recalibrate (time this), repeat A.

## Measures

- Detection accuracy, false activations/min, missed intents, latency from onset ([metrics](../../docs/metrics.md))
- Recalibration time
- [intent-control](../instruments/intent-control.md)

## Success criteria / gate

The Phase 2 engineering targets (project choices, not biological laws):
- **> 95 %** reliable rest/flex detection
- **< 1 false activation/min** at rest
- **< 150 ms** perceived command latency
- Recalibration **< 30 s**
- **Still works after the electrodes are removed and reattached**

Meeting all five completes **V0** together with 000 and 002.

## Safety

See [safety](../../docs/safety.md). Battery power only while the electrodes are on the body. Follow the manufacturer's electrode placement and avoid sensitive locations.

## Analysis plan

Confusion matrix per condition. Detection latency histogram. Compare accuracy in A and B to measure robustness to reattachment.

## Notes / open questions

- How does accuracy change with k? Record the ROC.
- Raw EMG features are deferred to 004.
