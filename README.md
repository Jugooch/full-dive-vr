# full-dive-vr

A long-running personal R&D program toward **non-invasive "partial-dive" VR**: lie in a sensory recliner
and walk, grab, fight, and *use magic* through subtle biological intent, while synchronized physical
feedback convinces the brain that the virtual body is yours.

> **Central question:** How little physical movement and conventional input can a person use while still
> experiencing strong agency, ownership and presence in a freely moving virtual body, and can that body
> be given abilities the physical body doesn't have?

True SAO-style full dive is blocked by neural *write* bandwidth and safe motor isolation, not graphics.
The research bet is that a hybrid (real eyes and ears, intent sensing, coherent touch, vibration and airflow)
can feel most of the way there long before direct neural interfaces exist. This repo is the platform
for testing that bet, one measurable experiment at a time, with **nothing invasive and no stimulation**.

## Where things are

| Path | What |
|---|---|
| [`docs/research/`](docs/research/) | The founding research record (feasibility → pod → DIY strategy → full guide → mana → incantations → V0–V6), 80 sources |
| [`docs/roadmap.md`](docs/roadmap.md) | **Start here.** V0–V6, two branches, gates, EEG track, status |
| [`docs/`](docs/) | Architecture, hardware guide, research protocol, metrics, **safety**, research log, decisions |
| [`experiments/`](experiments/) | 22 experiment protocols (`0xx` embodied control, `1xx` synthetic physiology, `2xx` integration) + instruments |
| [`schemas/`](schemas/) | Contracts between subsystems: `IntentFrame`, `HapticEvent`, `ChantPhrase`, `ManaSpell`, `block`, `session` |
| [`services/`](services/) | `partialdive` Python package: EMG → intent → Unreal, haptic bus, blinded sessions, metrics, CLI |
| [`firmware/`](firmware/) | ESP32: `emg-streamer` (MyoWare 2.0), `haptics-controller` (DRV2605L zones + fans) |
| [`unreal/`](unreal/) | Unreal 5.8 + OpenXR project (`PartialDiveVR`) and the `PartialDiveBridge` spec |
| [`hardware/`](hardware/) | Hardware profiles, BOM, wiring, pod/recliner design |
| [`data/`](data/) | Session metadata (committed) and recordings (not committed) |
| [`analysis/`](analysis/) | Notebooks and result reports |

## The plan in one table

| Version | Question |
|---|---|
| **V0** | Can subtle biological intent reliably control VR? |
| **V1** | Can I inhabit and locomote with minimal physical movement? |
| **V2** | Can I create a convincing fictional internal "mana core"? |
| **V2.5** | Can users learn to route that synthetic sensation? |
| **V3** | Can multiple learned bodily patterns represent different magic? |
| **V4** | Can chants improve control and learning? |
| **V5** | Can practice reduce reliance on haptics, movement and chants? |
| **V6** | Can locomotion + embodiment + synthetic physiology coexist naturally? |

EEG is a separate track that upgrades the intent source once Phases 1–4 work. See the [roadmap](docs/roadmap.md).

## Quick start (no hardware needed)

Run these in **PowerShell on Windows**, from the repo root, not in WSL: Unreal runs on Windows and
localhost UDP from WSL doesn't reliably reach it. Needs Python 3.11+ (`py --version`).

**One-time setup**

```powershell
cd "C:\Users\<you>\...\full-dive-vr"
py -3.12 -m venv .venv
.\.venv\Scripts\python.exe -m pip install -e "services[hardware,lsl,dev]"
.\.venv\Scripts\python.exe -m pytest services/tests        # expect: 24 passed
```

(Calling `.venv\Scripts\...` directly means you never need `Activate.ps1`, which PowerShell's execution
policy often blocks. If you do activate it, drop the `.\.venv\Scripts\` prefix below.)

**Everyday testing: one command + Play**

```powershell
.\.venv\Scripts\partialdive.exe dev
```

That starts everything the Unreal project talks to: a simulated EMG decoder (self-calibrating) streaming
intent on UDP 47800, and the haptic bus on UDP 47801 (dry-run: it prints the actuator commands instead
of driving hardware). One status line shows intent rate, values, active block and haptics. Ctrl+C stops it.

Then press **Play** in Unreal. The bridge's **debug overlay** (top-left, on by default in the editor)
shows whether intent is LIVE, every channel, the block code and the last haptic event. Type
`pdive.debug 0` in the console (` key) to hide it.

When real hardware arrives: `partialdive calibrate --profile v0-forearm --out <file>` once, then
`partialdive dev --profile v0-forearm --calibration <file>`.

Next: order the V0 hardware ([`docs/hardware.md`](docs/hardware.md)) and run experiment
[000](experiments/000-loop-latency/).

## Ground rules

1. **Safety boundary:** read-only sensing + physical feedback. No brain/nerve/vestibular stimulation, no
   restraints. [`docs/safety.md`](docs/safety.md)
2. **Measure, don't vibe:** randomized, blinded conditions; held-out days; every session logged.
   [`docs/research-protocol.md`](docs/research-protocol.md)
3. **Raw biosignals never reach gameplay**; only `IntentFrame`s do. [`docs/architecture.md`](docs/architecture.md)
