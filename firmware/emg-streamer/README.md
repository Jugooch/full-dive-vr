# emg-streamer

Samples up to 4 MyoWare 2.0 muscle sensors (ENV output) at 1 kHz on ESP32 ADC1 pins and streams
`E,<millis>,<env_mv0>,...` lines at 921600 baud: the ENV voltage **at the sensor, in millivolts** (the
firmware undoes the input divider using the chip's calibrated `analogReadMilliVolts`). Read by
`partialdive.biosignal.SerialEMGSource`, which normalizes by the profile's `emg.full_scale_mv` (4200).

## ⚠️ Input divider is required

The Power Shield's LiPo is **unregulated, 3.7 V nominal, 4.2 V fully charged** [S59], and ENV spans
**0–VIN** [S58]. An ESP32 pin must stay below ~3.3 V and reads accurately only up to ~2.45 V at 11 dB
attenuation. So every ENV line goes through a divider. Attenuation does *not* make the pin tolerate more.

```text
MyoWare ENV ──[ 12 kΩ ]──┬──► ESP32 GPIO 34/39/36/32 (ADC1)      4.2 V × 15/27 = 2.33 V
                         │
                     [ 15 kΩ ]        (optional 10 nF pin→GND)
                         │
MyoWare GND ─────────────┴──► ESP32 GND   (connect GND first; common ground required)
```

Use 1% resistors. If you build a different ratio, set `R_TOP_OHMS` / `R_BOTTOM_OHMS` in
`platformio.ini` build flags so the output stays in sensor millivolts. Wire to the MyoWare's
0.1" `VIN / GND / ENV` PTH pads (no Link Shield needed). Follow
[`hardware/bringup-v0.md`](../../hardware/bringup-v0.md): **measure with a multimeter before connecting**.

The research recommends starting with the MyoWare **envelope** output rather than raw EMG: determine
whether rest vs. flex is separable with a threshold before any ML (experiment 003).

## Bill of materials (V0)

| Part | Qty | Approx. (2026) | Source |
|---|---|---|---|
| MyoWare 2.0 Muscle Sensor | 1 (V0), 3 (V1), +1 abdomen (V2) | ~$43 each | SparkFun [S31] |
| MyoWare 2.0 Power Shield (40 mAh LiPo built in) | **1 per sensor** | $15.95 | SparkFun DEV-21868 [S59] |
| 24 mm Ag/AgCl snap electrodes | 5 × 10-packs | ~$10 / 10 | SparkFun SEN-12969 |
| 1% resistors 12 kΩ + 15 kΩ | 1 pair per sensor | — | assortment |
| Olimex USB-ISO isolator | 1 | ~$21 | Olimex / DigiKey / Mouser |
| ESP32 board | 1 | ~$20 | Adafruit [S55] |

(Sources: [`docs/research/sources.md`](../../docs/research/sources.md).)

## Setup

1. Place electrodes **per the MyoWare 2.0 placement guide** (SparkFun Learn [S58]); don't improvise
   around sensitive locations.
2. Power sensors from the battery Power Shield (charged, **unplugged from USB-C**; never charge while worn).
   ESP32 → PC through the **Olimex USB-ISO** (never its external power jack, which is not isolated) or a laptop on battery.
3. Build and verify the divider (above), set `NUM_CHANNELS` in `src/main.cpp`, then `pio run -t upload`.
4. `pio device monitor` should show `E,<ms>,<mV>` lines; flexing should raise the value clearly above rest.
5. Edit the hardware profile's `emg.port` and `channels`, then `partialdive calibrate --profile <name> --out ...`.

## Future

- Wireless (BLE) variant for cable-free reclining — removes the mains-coupling path entirely.
- Raw EMG mode (MyoWare RAW pin) for experiment 004 feature work if ENV proves too smoothed.
