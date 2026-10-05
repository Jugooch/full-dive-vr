# Hardware profiles

A profile describes one physical setup: which sensors exist, where they are placed, which serial ports
they use, which EMG channel drives which intent, and which actuator channel sits on which body zone.

- `partialdive session init <exp> --profile <name>` records the profile name in `session.yaml`.
- Re-wiring or re-placing electrodes = **new profile file** (or edit + commit), never a firmware change.
- Ports differ per machine (`COM5` on Windows, `/dev/ttyUSB0` on Linux); edit them locally.

| Profile | Stage | What it has |
|---|---|---|
| `dev-simulated` | any | simulated EMG, dry-run haptics; for building without hardware |
| `v0-forearm` | V0 | 1 forearm EMG, 4 vibration zones |
| `v1-locomotion` | V1 | forearm + 2 leg EMG, 8 vibration zones, bass shaker, fans |
| `v2-mana` | V2+ | adds abdominal (core) EMG and a core→hand actuator path |

`intent_mapping` modes: `binary` (threshold T = μ_rest + kσ_rest with hysteresis), `continuous`
(C ∈ [0,1]), `cadence` (alternating left/right onsets → `walk_forward`). See
`services/src/partialdive/intent/decoder.py`.
