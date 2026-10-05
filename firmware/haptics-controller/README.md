# haptics-controller

ESP32 firmware driving vibration zones (DRV2605L haptic drivers + ERM/LRA motors) and fans.
Speaks [`haptic-command/1`](../../schemas/haptic-command.md) over USB serial at 115200 baud.

## Bill of materials (V0: 4 zones)

| Part | Qty | Notes |
|---|---|---|
| ESP32 dev board (e.g. Adafruit HUZZAH32 Feather) | 1 | ~$20 |
| Adafruit DRV2605L haptic driver (#2305) | 4 (→8 in Phase 4) | ~$8 each; waveform library + real-time mode |
| TCA9548A I2C multiplexer | 1 (+1 at 0x71 for >8 zones) | Required: every DRV2605L is fixed at address 0x5A |
| Adafruit 10 mm ERM disc motor (#1201) | 6 (4 + 2 spares) | ~$2 each; 2.5–3.8 V rated, ~60 mA @ 3 V |
| Velcro straps / sleeves | — | Zone placement: forearms, chest, shoulder (V0) |

Phase 4 airflow: fans on GPIO 25/26/27 **through a MOSFET or fan driver** (never directly from a GPIO),
separate 12 V supply with common ground.

## Power

Motor current flows through each DRV2605L's **VIN**, so size that rail, not the ESP32's 3.3 V regulator.

| Stage | DRV2605L VIN from | Budget |
|---|---|---|
| V0 bench (4 zones) | Feather **USB pin (5 V from USB)** | 4 × ~60–80 mA motors + logic: well within USB's 500 mA |
| Phase 4+ (8–16 zones, fans) | **Dedicated 5 V ≥ 2 A supply**, grounds common with the ESP32 | Fans stay on their own 12 V supply via MOSFETs |

Firmware sets the ERM **overdrive clamp** explicitly (`ERM_OD_CLAMP 0x88` ≈ 3.0 V), so 3 V-rated motors
stay within spec on a 5 V rail. This board needs **no** USB isolation (nothing on it is electrically
connected to the body), and it must not share wiring with the EMG chain.

Zones 9–16: add a second TCA9548A with its A0 jumper set (address **0x71**). Firmware 0.2.0 auto-detects it;
channels 8–15 map to its ports 0–7.

## Wiring

```text
ESP32 SDA/SCL ── TCA9548A (0x70) ── port0 ── DRV2605L ── motor  (zone per hardware profile)
                                   ├─ port1 ── DRV2605L ── motor
                                   ├─ ...
                                   └─ port7
ESP32 GPIO25/26/27 ── MOSFET gate ── fan(-) ; fan(+) ── 12 V
```

Which mux port is which body zone is defined in `hardware/profiles/*.yaml` (`haptics.zones`), not here.

## Check it

```bash
pio run -t upload && pio device monitor     # expect: ID haptics-controller 0.2.0 channels=4 fans=3 muxes=1
# in the monitor:  H 0 200 150   -> channel 0 buzzes for 150 ms
partialdive haptic-test right_forearm --profile v0-forearm
```

## Safety limits (enforced here, not configurable at runtime)

- 2000 ms max on-time per pulse; 5 s command watchdog; `S` stops everything.
- Thermal output (heating/cooling) is **out of scope** for this board — see `docs/safety.md`.
