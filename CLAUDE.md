# CLAUDE.md

Guidance for AI assistants working in this repo.

## What this is
A long-running self-experimentation research platform for non-invasive "partial-dive" VR (see README).
Founding research: `docs/research/` (a faithful record of the original conversation, so **don't rewrite it**
to reflect new thinking; put changes in `docs/decisions/` and `docs/research-log.md`).

## Hard rules
- `docs/safety.md` is binding. Never add code, firmware, docs or BOM entries for electrical stimulation of the
  brain, nerves or vestibular system, restraints, sleep induction, or breath-holding mechanics. Changing
  the boundary needs an ADR first, written by the user.
- Gameplay consumes only `IntentFrame`s. Raw biosignals never go to Unreal.
- Experiment conditions are applied in the haptic bus / via `block/1` params, randomized by
  `partialdive session init`. Never add code that lets a participant see or choose the condition.
- Schemas in `/schemas`, Python mirrors in `services/src/partialdive/contracts/` and the Unreal bridge change
  together; `services/tests/test_contracts.py` checks Python ↔ schema parity.
- Never commit raw recordings (`*.xdf`, audio, video). Never edit existing session data.

## Conventions
- Experiment IDs: `0xx` embodied control (010–013 EEG track), `1xx` synthetic physiology, `2xx` integration.
  New experiment = copy `experiments/_template/`, new ID, add it to `experiments/README.md` and `docs/roadmap.md`.
- Hardware specifics (ports, placements, zone→channel) live in `hardware/profiles/*.yaml`, not in code or firmware.
- Firmware: dumb and safe (channel-level commands, hard caps on the MCU). Bump `FW_VERSION` on behaviour changes.
- Metrics are defined once in `docs/metrics.md` + `partialdive.analysis`; reuse, don't redefine.
- Sources are cited `[S#]` per `docs/research/sources.md`.

## Commands
```bash
pip install -e "services[hardware,lsl,dev]"
pytest services/tests
partialdive -h
```
Firmware: PlatformIO (`pio run -t upload` in `firmware/<project>/`). Unreal: UE 5.8, created from the VR template.
