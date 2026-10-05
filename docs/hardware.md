# Hardware guide

What to buy, in what order, and why. Prices are **as found in the research (2026)**, so re-check them
before buying. `[S#]` refers to [research/sources.md](research/sources.md). The machine-readable list is
[`hardware/bom.csv`](../hardware/bom.csv); build artifacts (wiring, pod CAD) live in [`hardware/`](../hardware/).

> Budget principle from the research: **don't dump $2,000 into EEG first.** Prove the interaction with
> $50–200 of EMG and haptics. The custom electronics for V0–V1 (excluding VR) come to *well under a few
> hundred dollars*. EEG is where costs jump.

## Stage 0–1: platform + V0 (≈ $150–250 if you already own a PC + headset)

| Item | Why | Approx. | Ref |
|---|---|---|---|
| VR-capable PC | Unreal 5.8 + PCVR | owned | — |
| VR headset, Quest 3-class | Visual world; Link + OpenXR. If you already own decent PCVR, use it; don't upgrade yet | owned / Quest 3 | [S47, S48] |
| Comfortable reclining chair | Body support; experiments are reclined | used / owned | — |
| MyoWare 2.0 Muscle Sensor ×1 | Intent sensing. Raw, rectified **and envelope** outputs, adjustable gain | ~$43 | [S31, S57] |
| MyoWare 2.0 Power Shield | Battery power: biosignal electronics stay isolated | ~$16 | [S59] |
| Snap electrodes | Consumable | ~$10 / 10 | [S57] |
| ESP32 board ×2 | One EMG streamer, one haptics controller | ~$20 each | [S55] |
| DRV2605L haptic driver ×4 | Configurable haptic waveforms, not just on/off | ~$8 each | [S54] |
| Vibration motors (ERM/LRA) ×4 | Spatial touch: forearms, chest, shoulder | ~$2 each | — |
| TCA9548A I²C mux | *(added during repo setup)* All DRV2605Ls share address 0x5A | ~$7 | — |
| USB isolator (ADuM-based) | *(added during repo setup)* Lets the EMG ESP32 connect to a mains-powered PC safely. Alternative: laptop on battery | ~$20–40 | — |

Wireless alternative: MyoWare 2.0 Wireless Kit, ~$125 [S32].

## Stage 2–3: V1 multi-intent + locomotion (+ ≈ $100–150)

| Item | Why | Approx. |
|---|---|---|
| MyoWare 2.0 ×2 more (legs) + electrodes | Alternating leg activation → walking | ~$86 + consumables |
| DRV2605L + motors ×4 more | 8 zones: L/R shoulder, forearm, torso, thigh | ~$40 |

## Stage 4: sensory recliner v1 (+ ≈ $150–300)

Modalities in the research's order: **localized vibration → whole-body low-frequency impact → directional airflow → (optional) commercial vest**.

| Item | Why | Approx. | Ref |
|---|---|---|---|
| Dayton Audio BST-1 tactile transducer + small amp | 50 W, 4 Ω; mounts structurally to the chair; footsteps, explosions, impacts, landing | $55–78 + amp | [S61] |
| 3–4 PC/blower fans + MOSFET driver + 12 V supply | Front/left/right/(overhead) airflow; speed tracks avatar velocity | ~$40 | — |
| Used recliner / zero-gravity chair, foam, arm supports | **Support, not restraint**; immediate physical exit | varies | — |
| *Optional:* bHaptics TactSuit Air / Pro / TactSleeve | Quick answer to "does dense body haptics help?" but less control over parameters than custom motors | ~$320 / ~$565 / ~$225 | [S62] |

Prefer custom motors for **research**, because you control location, timing, amplitude, firmware and condition.

## Stage V2+: synthetic physiology (+ ≈ $60–100)

| Item | Why |
|---|---|
| MyoWare 2.0 for abdomen (core) | Gather: core activation C ∈ [0,1] |
| 2–6 extra motors on the core → chest → shoulder → arm → hand path | Traveling "mana flow"; experiment 102 tests how few are needed |
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

## Never buy / build (see [safety.md](safety.md))

Anything that puts current into the head, nerves or vestibular system (tDCS/tACS kits, TENS repurposing,
GVS); body restraints or motorized limb immobilization; thermal actuators before everything else works.
