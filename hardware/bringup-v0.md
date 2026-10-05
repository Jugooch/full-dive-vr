# V0 hardware bring-up checklist

Do these **in order**, one subsystem at a time, so every failure has one or two possible causes instead of
"VR + serial + EMG + four haptics + body placement". Tick each box in your research-log entry.
Safety rules: [`docs/safety.md`](../docs/safety.md). Parts: [`bom.csv`](bom.csv) stages 1–2.

## 0. Before anything touches a person
- [ ] Power Shield charged, then **unplugged from USB-C**. Never charge while it is attached to the sensor or worn.
- [ ] Multimeter available. Nothing below is connected to a person until step 3.

## 1. MyoWare alone (no ESP32)
- [ ] Snap MyoWare 2.0 onto the Power Shield; switch on. VIN LED lights.
- [ ] Electrodes on the forearm **per the MyoWare 2.0 placement guide** (SparkFun Learn [S58]).
- [ ] Measure ENV→GND with the multimeter: at rest, then a firm flex. Note the max (expect ≤ VIN ≤ 4.2 V).
- [ ] Remove the electrodes from your arm before step 2.

## 2. Input divider on the bench (one per ENV line)
- [ ] Breadboard: ENV ── 12 kΩ ──┬── ADC node ── 15 kΩ ── GND (optional 10 nF node→GND).
- [ ] With the MyoWare powered but **not** connected to the ESP32: put a known voltage on the divider input
      (e.g. the Power Shield VIN pad, ~3.7–4.2 V) and measure the ADC node. Expect VIN × 15/27
      (4.2 V → 2.33 V). It must read **< 2.5 V**. If not, stop and fix the resistors.

## 3. EMG → ESP32 → PC
- [ ] Flash `firmware/emg-streamer` (`NUM_CHANNELS = 1`). Plug the ESP32 into the PC **through the Olimex
      USB-ISO** (not its power jack), or into a laptop running on battery.
- [ ] Wire MyoWare GND → ESP32 GND **first**, then the divider's ADC node → GPIO 34 (A2).
- [ ] `pio device monitor`: lines `E,<ms>,<mV>` at 1 kHz. Values are sensor millivolts (divider undone).
- [ ] Electrodes back on. Rest vs. flex is clearly separated (rest roughly tens–hundreds of mV, flex much higher).
- [ ] Set `emg.port` in `hardware/profiles/v0-forearm.yaml`, then
      `partialdive calibrate --profile v0-forearm --out data/scratch/v0-cal.yaml`.

## 4. One haptic channel
- [ ] Flash `firmware/haptics-controller`. One DRV2605L on mux port 0, one motor.
      DRV VIN from the Feather **USB (5 V) pin**, grounds common. Monitor shows `ID haptics-controller 0.2.0 channels=1 ...`.
- [ ] In the monitor: `H 0 200 150` buzzes. `S` stops.
- [ ] `partialdive haptic-test right_forearm --profile v0-forearm` buzzes.

## 5. All four haptic zones
- [ ] Add the other three DRV2605L boards on mux ports 1–3. `I` reports `channels=4`.
- [ ] Fire all four together for 1 s; the ESP32 must not brown out/reset. (V0 budget: 4 × ~60 mA motors from
      the 5 V USB pin is fine; Phase 4's 8 zones move to a dedicated 5 V ≥ 2 A rail.)
- [ ] Strap motors to the forearms/chest/shoulder with soft sleeves; check zone names match the profile.

## 6. Full loop (experiment 000)
- [ ] `partialdive dev --profile v0-forearm --calibration data/scratch/v0-cal.yaml`, press Play in Unreal,
      overlay shows intent LIVE from `emg`.
- [ ] Flex → virtual hand closes → haptic fires on the forearm.
- [ ] Run [000](../experiments/000-loop-latency/) (software timing first, then with the piezo for the final gate).

Only then: strap on all four actuators for [002](../experiments/002-synchronized-touch/), then
[003](../experiments/003-emg-binary-intent/).
