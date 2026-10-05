# haptics-controller

ESP32 firmware driving vibration zones (DRV2605L haptic drivers + ERM/LRA motors) and fans.
Speaks [`haptic-command/1`](../../schemas/haptic-command.md) over USB serial at 115200 baud.

## Bill of materials (V0: 4 zones)

| Part | Qty | Notes |
|---|---|---|
| ESP32 dev board (e.g. Adafruit HUZZAH32 Feather) | 1 | ~$20 |
| Adafruit DRV2605L haptic driver | 4 (→8 in Phase 4) | ~$8 each; waveform library + real-time mode |
| TCA9548A I2C multiplexer | 1 | Required: every DRV2605L is fixed at address 0x5A |
| ERM vibration motors (or LRAs, build with `-D USE_LRA`) | 4 (→8) | ~$2 each |
| Velcro straps / sleeves | — | Zone placement: forearms, chest, shoulder (V0) |

Phase 4 airflow: fans on GPIO 25/26/27 **through a MOSFET or fan driver** (never directly from a GPIO),
separate 12 V supply with common ground.

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
pio run -t upload && pio device monitor     # expect: ID haptics-controller 0.1.0 channels=4 fans=3
# in the monitor:  H 0 200 150   -> channel 0 buzzes for 150 ms
partialdive haptic-test right_forearm --profile v0-forearm
```

## Safety limits (enforced here, not configurable at runtime)

- 2000 ms max on-time per pulse; 5 s command watchdog; `S` stops everything.
- Thermal output (heating/cooling) is **out of scope** for this board — see `docs/safety.md`.
