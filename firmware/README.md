# Firmware

ESP32 firmware, built with [PlatformIO](https://platformio.org/) (`pio run -t upload` inside a project
folder, or the PlatformIO VS Code extension).

| Project | Board | Purpose | Introduced |
|---|---|---|---|
| [`emg-streamer/`](emg-streamer/) | ESP32 (e.g. HUZZAH32 Feather) | Sample MyoWare 2.0 ENV outputs at 1 kHz → USB serial | V0 |
| [`haptics-controller/`](haptics-controller/) | ESP32 + TCA9548A + DRV2605L ×N | Vibration zones + fan PWM, `haptic-command/1` protocol | V0 (fans: Phase 4) |

Design rules:
- **Firmware is dumb and safe.** No experiment logic, no zone names, no conditions. Those live in
  `services/` and `hardware/profiles/`. Firmware only executes channel-level commands and enforces
  hard safety limits (max on-time, command watchdog, `S` = stop all).
- **Biosignal hardware is never mains-coupled while on the body.** See [`docs/safety.md`](../docs/safety.md#electrical).
- Bump `FW_VERSION` on every behaviour change; `partialdive` records it in `session.yaml`.
- Future boards (EEG is handled by vendor hardware + BrainFlow, not custom firmware; respiration sensor;
  pod pressure actuators) get their own folder here with the same rules.
