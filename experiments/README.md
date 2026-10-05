# Experiments

Each experiment answers **one question** that the eventual partial-dive pod depends on. Each one has a written protocol (`README.md`) and a machine-readable condition set (`conditions.yaml`). The tooling reads the condition set to randomize and log sessions.

> Central research question: **How little physical movement and conventional input can a person use while still experiencing strong agency, ownership, and presence in a freely moving virtual body?**
> — [04-research-program-guide](../docs/research/04-research-program-guide.md)

## Numbering

| Range | Branch | What it covers |
|---|---|---|
| `0xx` | **Embodied Control** | Intent loop, virtual body ownership, EMG intent, motionless locomotion, sensory recliner, EEG track |
| `1xx` | **Synthetic Physiology** | Mana core, tactile flow, learned sensation, magic control language, incantations, silent casting |
| `2xx` | **Integration** | Merging both branches into one embodied experience |

Both branches run on the same platform: biosignals → Intent Bus → Unreal → Haptic Bus → body. Synthetic-physiology work doesn't wait for locomotion to be "finished". See [07-version-roadmap](../docs/research/07-version-roadmap.md).

Each directory is `experiments/<id>-<slug>/`. IDs are never reused. If an experiment changes materially, create a new ID and mark the old one `archived`.

## Lifecycle

`planned` → `active` → `complete` → `archived`

- **planned**: the protocol is written. Hardware and software may not exist yet.
- **active**: sessions are being collected. Freeze the protocol and record any change in *Notes* with a date.
- **complete**: the gate in *Success criteria / gate* has been evaluated (passed or failed) and the result is written up in [`docs/research-log.md`](../docs/research-log.md).
- **archived**: superseded or abandoned. Keep the directory and give the reason.

## Running a session

```bash
partialdive session init <experiment-id> --profile <hardware-profile>   # e.g. 002 --profile v0-forearm
partialdive block start <session-dir> <n>    # before each block: sends params to Unreal + haptic bus, marks LSL
partialdive session unblind <session-dir>    # only after all data is collected
```

`randomize` in `conditions.yaml` may be `true` (balanced within rounds), `full` (one global shuffle,
for unpredictable single-trial blocks such as 104's catch casts) or `false` (fixed order, for
training progressions such as 007). A block can be a whole run or a single trial or cast.

This creates `data/sessions/<experiment-id>/<YYYY-MM-DD>_sNN/session.yaml` with:

- the git commit of this repo (and of the Unreal project and firmware, if separate);
- the active hardware profile (headset, EMG, haptics revision, etc.);
- the **randomized condition order**, generated from `conditions.yaml`. When `blinded: true`, the order is kept off the participant display;
- slots for metrics, questionnaire scores and notes.

Raw recordings (XDF via LSL/LabRecorder) go next to `session.yaml` in `data/`. Large raw data never goes into ordinary git history (see [`data/README.md`](../data/README.md)).

## Rules

1. **Never self-select conditions.** The randomizer chooses the order. Don't look at which condition is running while you're inside it.
2. **Held-out evaluation.** Train or calibrate on some runs or days and evaluate on others (e.g. runs 1–3 to train, run 4 to validate; Monday to train, Tuesday to evaluate). Never randomly split adjacent windows of a biosignal recording.
3. **Establish your own variance.** Repeat baselines on different days before claiming an effect.
4. **Measure; don't rely on "it felt better".** Every session records metrics and questionnaires (see [`instruments/`](instruments/) and [`docs/metrics.md`](../docs/metrics.md)).
5. **Log every session** in [`docs/research-log.md`](../docs/research-log.md), failures included.
6. **Safety boundary**: read-only biosensing plus physical feedback only. No electrical stimulation of brain, nerves or vestibular system. Support the body, never restrain it. See [`docs/safety.md`](../docs/safety.md).

## Index

| ID | Name | Version / Phase | Depends on | Status |
|---|---|---|---|---|
| [000](000-loop-latency/) | End-to-end loop latency | V0 | — | planned |
| [001](001-baseline-embodiment/) | Baseline embodiment | V0 · Phase 1A | — | planned |
| [002](002-synchronized-touch/) | Synchronized touch | V0 · Phase 1B | 000, 001 | planned |
| [003](003-emg-binary-intent/) | EMG binary intent | V0 · Phase 2A | 000 | planned |
| [004](004-emg-multi-intent/) | EMG multi-intent classification | V1 · Phase 2B | 003 | planned |
| [005](005-emg-walking/) | EMG walking | V1 · Phase 3A | 004 | planned |
| [006](006-emg-vs-joystick-locomotion/) | EMG vs joystick locomotion | V1 · Phase 3B | 005, 001 | planned |
| [007](007-minimal-activation/) | Minimal activation | V1 · Phase 3C | 006 | planned |
| [008](008-sensory-ablation/) | Sensory ablation | V1 · Phase 4A | 002, 005 | planned |
| [009](009-sword-embodiment/) | Sword embodiment | V1 · Phase 4B | 004, 008 | planned |
| [010](010-eeg-motor-execution/) | EEG motor execution | EEG track · Phase 5A | 007 (soft) | planned |
| [011](011-eeg-motor-imagery/) | EEG motor imagery | EEG track · Phase 5B | 010 | planned |
| [012](012-eeg-in-vr/) | EEG in VR | EEG track · Phase 5C | 011 | planned |
| [013](013-multimodal-intent-fusion/) | Multimodal intent fusion | EEG track · Phase 6 | 012, 009 | planned |
| [101](101-mana-core-loop/) | Mana core loop | V2 | 003, 002 | planned |
| [102](102-tactile-flow-illusion/) | Tactile flow illusion | V2 | 002 | planned |
| [103](103-embodied-vs-arbitrary-casting/) | Embodied vs arbitrary casting | V2 | 101 | planned |
| [104](104-learned-mana-sensation/) | Learned mana sensation | V2.1 / V2.5 | 101, 102 | planned |
| [105](105-mana-control-language/) | Mana control language | V3 | 104 | planned |
| [106](106-incantations/) | Incantations | V4 | 105 | planned |
| [107](107-silent-casting-proficiency/) | Silent-casting proficiency | V5 | 106 | planned |
| [201](201-locomotion-plus-magic/) | Locomotion + magic | V6 | 007, 107 | planned |

The EEG track (010–013) can upgrade **any** version's intent source once it works, because every decoder publishes the same `IntentFrame`.

## Starting a new experiment

```bash
cp -r experiments/_template experiments/<id>-<slug>
```

Fill in every section of the template before the first session. Add the experiment to the index above.
