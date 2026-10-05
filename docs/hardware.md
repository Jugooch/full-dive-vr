# Hardware guide

What to buy, in what order, and why. Prices are **as found in the research (2026)**, so re-check them
before buying. `[S#]` refers to [research/sources.md](research/sources.md). The machine-readable list is
[`hardware/bom.csv`](../hardware/bom.csv); build artifacts (wiring, pod CAD) live in [`hardware/`](../hardware/).

> Budget principle from the research: **don't dump $2,000 into EEG first.** Prove the interaction with
> $50–200 of EMG and haptics. The custom electronics for V0–V1 (excluding VR) come to *well under a few
> hundred dollars*. EEG is where costs jump.

## Stage 0–1: platform + V0 (≈ $290 of electronics + tools you may already own)

Order this now. Bring it up with [`hardware/bringup-v0.md`](../hardware/bringup-v0.md), one subsystem
at a time, before anything goes in VR.

| Item | Why | Approx. | Ref |
|---|---|---|---|
| VR-capable PC | Unreal 5.8 + PCVR | owned | — |
| VR headset: **Oculus Rift S** (owned) | Visual world via OpenXR (Meta PC app as runtime). The research says use the PCVR headset you have; a Quest 3 is *not* required. Inside-out tracking (no external sensors), built-in mic for V4, Touch controllers for baselines | owned | [S47, S48] |
| Comfortable reclining chair | Body support; experiments are reclined | owned | — |
| **MyoWare 2.0 Muscle Sensor** ×1 (buy direct from SparkFun) | Intent sensing. Raw, rectified **and envelope** outputs, adjustable gain | ~$43 | [S31, S57] |
| **MyoWare 2.0 Power Shield** (DEV-21868) | Battery power keeps the body-connected chain off mains. 40 mAh LiPo is **built in** (don't buy another), **unregulated, up to 4.2 V**. Unplug USB-C before wearing; never charge while worn | $15.95 | [S59] |
| **24 mm Ag/AgCl electrodes** ×5 packs (SEN-12969) | Single-use; experiment 003 deliberately removes/reattaches them | ~$10 / 10 | [S57] |
| **1% resistor assortment** | **Required** ENV → ESP32 divider (12 kΩ / 15 kΩ): ENV spans 0–VIN (≤ 4.2 V), the ESP32 pin must stay < 3.3 V | ~$12 | [S58, S59] |
| **Adafruit HUZZAH32 ESP32 Feather** ×2 | One EMG streamer, one haptics controller. Exactly the board the firmware targets (`featheresp32`) | ~$20 each | [S55] |
| **Adafruit DRV2605L** (#2305) ×4 | Configurable haptic waveforms, not just on/off | ~$8 each | [S54] |
| **TCA9548A I²C mux** breakout | All DRV2605Ls share address 0x5A | ~$9 | — |
| **Adafruit 10 mm ERM disc motor** (#1201) ×6 | 4 zones + 2 spares; 2.5–3.8 V rated, ~60 mA @ 3 V | ~$2 each | — |
| **Olimex USB-ISO** | Real galvanic isolation (1000 VDC, USB Full Speed; plenty for the serial stream) between the EMG ESP32 and a mains-powered PC. **Never use its external power jack**, which is not isolated from the PC side. Alternative: laptop on battery | ~$21 | — |
| **Piezo disc** (or small accelerometer) ×2 | Physical ground truth for actuator onset. **Required** for experiment 000's final latency gate | ~$3 | — |
| Breadboard + jumper kit, 0.1" headers, hook-up wire | Bring-up before soldering anything wearable; MyoWare VIN/GND/ENV pads | ~$23 | — |
| USB cables: 2× micro-USB **data** (HUZZAH32), 1× USB-C (Power Shield charging) | Easy to discover you lack one | ~$15 | — |
| Velcro straps / soft elastic sleeves | Removable motor and electronics mounting | ~$12 | — |
| Digital multimeter *(if not owned)* | Measure ENV max and divider output **before** connecting the ESP32 or a person | ~$20–40 | — |
| Soldering iron, solder, heat-shrink *(if not owned)* | Permanent wiring after breadboard validation | varies | — |

**Haptic power:** motor current flows through each DRV2605L's VIN, not the ESP32's 3.3 V regulator. For V0,
feed DRV VIN from the Feather's USB (5 V) pin. Firmware clamps drive to ≈ 3.0 V so the 3 V motors stay in
spec. The haptic ESP32 needs no isolation; only the electrode-connected chain does.

Wireless alternative: MyoWare 2.0 Wireless Kit, ~$125 [S32].

## Stage 2–3: V1 multi-intent + locomotion (+ ≈ $150)

| Item | Why | Approx. |
|---|---|---|
| MyoWare 2.0 ×2 more (legs) | Alternating leg activation → walking | ~$86 |
| **MyoWare 2.0 Power Shield ×2 more** | **One per sensor**: each EMG stack needs its own battery to stay isolated | ~$32 |
| Electrodes ×5 packs, 2 more divider resistor pairs (from the assortment) | Consumables, ENV protection per sensor | ~$50 |

## Stage 4: sensory recliner v1 (+ ≈ $150–300)

Modalities in the research's order: **localized vibration → whole-body low-frequency impact → directional airflow → (optional) commercial vest**.

| Item | Why | Approx. | Ref |
|---|---|---|---|
| DRV2605L ×4 + disc motors ×6 | Zones 5–8 (L/R shoulder, forearm, torso, thigh) + spares | ~$45 | [S54] |
| **Dedicated 5 V ≥ 2 A supply** (common ground) | Haptic rail for 8 zones; don't run 8 motors from the Feather's USB pin | ~$15 | — |
| Dayton Audio BST-1 tactile transducer + small amp | 50 W, 4 Ω; mounts structurally to the chair; footsteps, explosions, impacts, landing | $55–78 + amp | [S61] |
| 3–4 PC/blower fans + MOSFET driver + 12 V supply | Front/left/right/(overhead) airflow; speed tracks avatar velocity | ~$40 | — |
| Used recliner / zero-gravity chair, foam, arm supports | **Support, not restraint**; immediate physical exit | varies | — |
| *Optional:* bHaptics TactSuit Air / Pro / TactSleeve | Quick answer to "does dense body haptics help?" but less control over parameters than custom motors | ~$320 / ~$565 / ~$225 | [S62] |

Prefer custom motors for **research**, because you control location, timing, amplitude, firmware and condition.

## Stage V2+: synthetic physiology (+ ≈ $60–100)

| Item | Why |
|---|---|
| MyoWare 2.0 for abdomen (core) **+ its own Power Shield** | Gather: core activation C ∈ [0,1] |
| 2–6 extra motors on the core → chest → shoulder → arm → hand path | Traveling "mana flow"; experiment 102 tests how few are needed |
| **Second TCA9548A at 0x71** *(or reuse existing chest/shoulder/forearm zones)* | V1's 8 zones + the flow path exceed one mux; firmware 0.2.0 supports a second mux |
| *Optional:* respiration / stretch sensor | Breathing as a mana signal (never breath-holding) |
| Headset microphone (built in) | V4 incantations |

## EEG track (only after Phases 1–4 produce useful results)

Motor-imagery EEG needs electrodes over sensorimotor cortex (**C3, Cz, C4** in the 10–20 system).

| Option | Specs | Approx. | Verdict | Ref |
|---|---|---|---|---|
| **OpenBCI Ganglion** | 4 ch, 200 Hz, BLE, open | $624.99 + electrodes/headgear | **First choice.** 4 channels are enough to explore C3/Cz/C4 | [S65, S66] |
| OpenBCI Cyton | 8 ch, 250 Hz | $1,249 board | If EEG becomes a serious interest | [S39, S67] |
| OpenBCI Ultracortex (complete) | headset + board | ~$2,999 | Later / optional | [S40] |
| Neurosity Crown | 8 ch, 256 Hz, dry, C3/C4/CP3/CP4, SDK | $1,499 | Convenient, less placement freedom | [S38, S69] |
| ~~Muse 2~~ | TP9/AF7/AF8/TP10 | ~$250 | **Don't buy for this:** sensors are mostly not over motor cortex | [S37] |

All of the above are supported through **BrainFlow** [S36, S41]. OpenBCI specifies battery-only operation for Cyton [S60];
PiEEG warns its hardware is not a medical device and must be fully isolated from mains [S44].

**Not yet a complete EEG BOM:** once a board is chosen, add its electrodes/headgear, paste or gel, and
cables as their own rows.

## Gaze (Phase 6 fusion)

The Rift S has **no eye tracking**. Until a later stage specs eye-tracking hardware, Phase 6 "gaze" means
head direction. Give it its own hardware stage and ADR when fusion work begins.

## Never buy / build (see [safety.md](safety.md))

Anything that puts current into the head, nerves or vestibular system (tDCS/tACS kits, TENS repurposing,
GVS); body restraints or motorized limb immobilization; thermal actuators before everything else works.
