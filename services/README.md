# services — `partialdive` Python package

The signal side of the platform: acquisition → intent decoding → IntentFrame → Unreal, and Unreal →
HapticEvent → haptic bus → actuators. Plus session tooling and shared metrics.

```bash
python -m venv .venv && . .venv/bin/activate        # Windows: .venv\Scripts\activate
pip install -e "services[hardware,lsl,dev]"          # add ml / eeg / analysis extras when needed
pytest services/tests
```

| Module | Purpose |
|---|---|
| `contracts` | Python mirrors of `/schemas` (`IntentFrame`, `HapticEvent`) |
| `biosignal` | `SerialEMGSource` (ESP32 emg-streamer), `SimulatedEMGSource` (no hardware needed) |
| `intent` | `ThresholdDetector` (T = μ+kσ, hysteresis, dwell), `ActivationNormalizer` (C ∈ [0,1]), `StepCadence` (v = k·f), features, profile-driven `EmgIntentDecoder` |
| `haptics` | `HapticBus` with `HapticCondition` (delay, remap, gating, flows, reduced/visual-only), serial transport |
| `session` | Blinded randomized sessions, sealed key, unblinding |
| `analysis` | Accuracy/F1, false activations/min, latency, Wolpaw ITR, effort ratio E/R, binomial above-chance, questionnaire subscales |
| `lsl`, `net` | LSL clock/outlets (optional), localhost UDP JSON |
| `voice` | V4 placeholder |

## CLI

```text
partialdive session init <exp> --profile <profile> [--participant P01]
partialdive session unblind <session_dir>
partialdive block start <session_dir> <n>
partialdive calibrate --profile <profile> --out <session_dir>/calibration.yaml [--simulate]
partialdive decode --profile <profile> --calibration <file> [--simulate]
partialdive haptic-bus --profile <profile> [--session-dir D --block N] [--dry-run]
partialdive haptic-test <zone> --profile <profile> [--dry-run]
```

No hardware yet? `--profile dev-simulated --simulate` runs the whole decode loop with synthetic EMG
(alternating legs walk, forearm grabs, abdomen gathers) so the Unreal bridge can be built first.
