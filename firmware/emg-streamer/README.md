# emg-streamer

Samples up to 4 MyoWare 2.0 muscle sensors (ENV output) at 1 kHz on ESP32 ADC1 pins and streams
`E,<millis>,<v0>,...` lines at 921600 baud. Read by `partialdive.biosignal.SerialEMGSource`.

The research recommends starting with the MyoWare **envelope** output rather than raw EMG: determine
whether rest vs. flex is separable with a threshold before any ML (experiment 003).

## Bill of materials (V0)

| Part | Qty | Approx. (2026) | Source |
|---|---|---|---|
| MyoWare 2.0 Muscle Sensor | 1 (V0), 3 (V1), +1 abdomen (V2) | ~$43 each | SparkFun [S31] |
| MyoWare 2.0 Power Shield (battery) | 1 per stack | ~$16 | SparkFun [S59] |
| Snap electrodes | packs | ~$10 / 10 | SparkFun |
| ESP32 board | 1 | ~$20 | Adafruit [S55] |

(Sources: [`docs/research/sources.md`](../../docs/research/sources.md).)

## Setup

1. Place electrodes **per the MyoWare 2.0 placement guide** (SparkFun Learn [S58]); don't improvise
   around sensitive locations.
2. Power sensors from the battery Power Shield. Laptop on battery / USB isolator (see firmware header).
3. Set `NUM_CHANNELS` and pins in `src/main.cpp`, then `pio run -t upload`.
4. `pio device monitor` should show `E,...` lines; flexing should raise the value clearly above rest.
5. Edit the hardware profile's `emg.port` and `channels`, then `partialdive calibrate --profile <name> --out ...`.

## Future

- Wireless (BLE) variant for cable-free reclining — removes the mains-coupling path entirely.
- Raw EMG mode (MyoWare RAW pin) for experiment 004 feature work if ENV proves too smoothed.
