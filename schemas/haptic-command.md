# Haptic command — serial line protocol (`haptic-command/1`)

The haptic bus (`partialdive.haptics`) talks to the ESP32 haptics controller over USB serial,
115200 baud, newline-terminated ASCII. ASCII is deliberate: you can drive and debug it from any serial
monitor without the Python stack.

## Host → controller

| Command | Meaning |
|---|---|
| `H <ch> <strength> <duration_ms>` | Fire actuator channel `ch` (0-based) at `strength` 0–255 for `duration_ms`. |
| `E <ch> <effect>` | Play DRV2605L library effect number `effect` (1–123) on channel `ch`. |
| `F <fan> <duty>` | Set fan `fan` PWM duty 0–255 (Phase 4 airflow). |
| `S` | Stop everything immediately (all channels off, fans off). |
| `P` | Ping. |
| `I` | Identify: report firmware version and channel count. |

## Controller → host

| Reply | Meaning |
|---|---|
| `OK` | Command accepted. |
| `ERR <reason>` | Rejected (bad channel, out of range, parse error). |
| `PONG <millis>` | Reply to `P` with controller uptime. |
| `ID haptics-controller <version> channels=<n> fans=<m>` | Reply to `I`. |

## Safety behaviour (firmware-enforced)

- Any channel is forced off after **2000 ms** max on-time regardless of the requested duration.
- If no command arrives for **5 s** while anything is active, the controller stops everything (watchdog).
- `S` is always honoured, even mid-pattern.

Zone → channel mapping is **not** in firmware. It lives in the hardware profile
(`hardware/profiles/*.yaml`) so re-wiring never needs a re-flash.
