# 04 — Staged Research Program Guide (Phases 0–6)

| | |
|---|---|
| **Source** | User question in the archived conversation (see [transcript](transcript.md)) |
| **Date of record** | 2026-10-05 |
| **Related** | [Roadmap](../roadmap.md) · [Architecture](../architecture.md) · [Hardware](../hardware.md) · [Research protocol](../research-protocol.md) · [Metrics](../metrics.md) · [Safety](../safety.md) · experiments [000-loop-latency](../../experiments/000-loop-latency/), [001-baseline-embodiment](../../experiments/001-baseline-embodiment/), [002-synchronized-touch](../../experiments/002-synchronized-touch/), [003-emg-binary-intent](../../experiments/003-emg-binary-intent/), [004-emg-multi-intent](../../experiments/004-emg-multi-intent/), [005-emg-walking](../../experiments/005-emg-walking/), [006-emg-vs-joystick-locomotion](../../experiments/006-emg-vs-joystick-locomotion/), [007-minimal-activation](../../experiments/007-minimal-activation/), [008-sensory-ablation](../../experiments/008-sensory-ablation/), [009-sword-embodiment](../../experiments/009-sword-embodiment/), [010-eeg-motor-execution](../../experiments/010-eeg-motor-execution/), [011-eeg-motor-imagery](../../experiments/011-eeg-motor-imagery/), [012-eeg-in-vr](../../experiments/012-eeg-in-vr/), [013-multimodal-intent-fusion](../../experiments/013-multimodal-intent-fusion/) |

**The question this answers:**

> Okay, I want to start on this, could you take all of the information you just found and help write up an actionable list of what I could do to start this experiment . You can essentially expand upon 1-5, giving suggestions for what technology could be bought and why, what experiments should be run, etc… basically a full research guide for starting from scratch and working on this sequentially

## Summary given up front

I’ll turn this into a staged research program rather than a shopping list: each phase will have a concrete hypothesis, minimum viable hardware, experiment protocol, success criteria, and a “don’t advance until this works” gate. I’m also going to keep the first several phases strictly **read-only biosensing + physical haptics**—no DIY brain/nerve/vestibular stimulation.

The first concrete conclusion is that the cheapest useful prototype is smaller than it sounds. A single current MyoWare EMG sensor is about **$43**, a Dayton BST-1 tactile transducer is roughly **$55–78**, and you can DIY localized vibration for far less than a commercial haptic suit; the expensive EEG gear should be deferred until the interaction model proves itself. 

For software, I’d standardize on **Unreal + OpenXR** and keep biosignals outside the game in a small acquisition/classification service. Meta’s current developer stack supports running Unreal scenes directly over Link, and Epic/Meta both recommend OpenXR for new XR work, so you can swap headsets later without rewriting the experimental core. 

Yes. I would treat this as a real **personal R&D program for non-invasive "partial-dive" VR**, with every stage answering one question that the eventual pod depends on.

The central research question I'd build around is:

> **How little physical movement and conventional input can a person use while still experiencing strong agency, ownership, and presence in a freely moving virtual body?**

That keeps the project scientifically meaningful even if true full-dive BCI remains decades away.

---

## 0. Build the research platform before the pod

Do this before buying EEG hardware or fabricating an enclosure.

Your architecture should stay modular:

```text
                 ┌──────────────────────┐
                 │      Unreal VR       │
                 │ avatar/world/physics │
                 └──────────┬───────────┘
                            │
                     Intent Interface
                            │
          ┌─────────────────┼─────────────────┐
          │                 │                 │
       Controller          EMG              EEG
       baseline           later             much later
          │                 │                 │
          └──────────┬──────┴───────┬─────────┘
                     │              │
                  LSL/logger    classifier
                     │              │
                     └──────┬───────┘
                            │
                       Haptic Bus
                            │
           ┌────────────────┼─────────────────┐
           │                │                 │
       vibration        bass shaker          fans
```

### Software stack

I'd use:

| Component | Recommendation |
|---|---|
| VR engine | **Unreal Engine 5** |
| XR API | **OpenXR** |
| Headset connection | PCVR / Meta Link if using Quest |
| Biosignal service | Python initially |
| ML | scikit-learn first, PyTorch later |
| Biosignal API | BrainFlow where supported |
| Experiment synchronization | **Lab Streaming Layer (LSL)** |
| Recording | XDF + experiment metadata |
| Analysis | Python / pandas / SciPy / MNE |
| Version control | Git |

Unreal 5.8 has a VR template and OpenXR support, and Meta currently recommends Epic Native OpenXR for new projects. Meta Link also lets you run directly from Unreal into a Quest headset while developing. [S45, S46, S47, S48]

LSL is worth adopting **immediately**, even before you own an EEG. It exists specifically to synchronize streams such as EEG, eye tracking, experimental events, etc.; its timing system is designed for sub-millisecond synchronization on a local network and records into XDF through LabRecorder. [S49, S50]

#### Repository

I'd start something like:

```text
partial-dive/
├── unreal/
│   └── PartialDiveVR/
├── firmware/
│   ├── haptics/
│   └── emg/
├── services/
│   ├── biosignal/
│   ├── intent/
│   └── lsl/
├── experiments/
│   ├── 001-baseline-embodiment/
│   ├── 002-haptic-latency/
│   └── ...
├── analysis/
├── data/
│   └── README.md
└── docs/
    ├── architecture.md
    ├── hardware.md
    ├── research-protocol.md
    └── research-log.md
```

Don't put large raw XDF recordings directly into ordinary Git history.

---

## Phase 1 — Establish VR body ownership

### Goal

Before neural interfaces, prove that you can make your brain strongly identify with a virtual body.

You want:

$$
\text{virtual body}
\rightarrow
\text{"this feels like my body"}
$$

rather than merely:

$$
\text{virtual character}
\rightarrow
\text{"I'm controlling this character"}
$$

This distinction becomes extremely important later.

Research supports measuring embodiment in terms of **ownership, agency, and change in perceived body schema**. The validated Virtual Embodiment Questionnaire (VEQ) gives you a ready-made measurement instrument. [S51, S52]

### Hardware

At minimum:

- VR headset;
- VR-capable PC;
- comfortable reclining chair;
- controllers initially;
- Unreal.

If you already own a decent PCVR headset, use it. Don't upgrade yet.

If starting completely from scratch, a Quest 3-class device is perfectly sufficient for this stage.

### VR prototype

Build **one tiny laboratory**, not a game.

Something like:

```text
room
│
├── mirror
├── table
├── cube
├── sphere
├── sword
└── target dummy
```

Give yourself a complete avatar:

- torso;
- arms;
- hands;
- legs;
- feet.

Seeing hands and feet can influence the onset and persistence of illusory body ownership, so don't build the experiment around floating controller hands. [S53]

Your first-person camera should be positioned where the avatar's eyes would actually be.

---

### Experiment 1A — baseline embodiment

Spend perhaps 5–10 minutes doing standardized actions:

```text
look at hands
↓
open/close hands
↓
look in mirror
↓
touch virtual table
↓
pick up cube
↓
move arms
↓
look down at body
```

Immediately afterward complete the VEQ.

Record:

```text
session
condition
duration
frame rate
headset
avatar model
VEQ ownership
VEQ agency
VEQ body-change
notes
```

This becomes your baseline.

Do this several times on different days.

You're establishing **your own variance**, not trying to publish a neuroscience paper yet.

---

## Phase 1B — synchronized touch

Now add the first "virtual → physical" signal.

This can be surprisingly inexpensive.

A DRV2605L haptic controller is currently about $8, and tiny vibration motors are roughly $2 each. The DRV2605L supports configurable haptic waveforms rather than simply switching a motor on and off. [S54]

Use something like:

```text
ESP32
   │
   ├── vibration motor → left forearm
   ├── vibration motor → right forearm
   ├── vibration motor → chest
   └── vibration motor → shoulder
```

An inexpensive ESP32 board is around $20. [S55]

When:

```text
virtual object touches right forearm
```

send:

```text
HAPTIC_RIGHT_FOREARM
strength=0.6
duration=120ms
```

---

### Experiment 1B

Test randomized conditions:

| Condition | VR contact | Physical feedback |
|---|---|---|
| A | right arm | immediate right arm |
| B | right arm | ~150 ms delayed right arm |
| C | right arm | >300 ms delayed right arm |
| D | right arm | immediate left arm |
| E | right arm | none |

Delayed visual/tactile feedback above roughly 300 ms has been shown to significantly weaken rubber-hand ownership effects, which makes this a useful region to explore experimentally. [S56]

Don't tell yourself which condition is running.

Randomize it programmatically.

Then collect VEQ scores.

You're looking for:

$$
\text{synchronous + spatially congruent}
>
\text{delayed/mismatched}
$$

in ownership.

If you can't observe *any* difference in yourself, fix this before proceeding.

---

## Phase 2 — Replace buttons with muscle intent

Now you begin moving toward "thinking about moving."

But don't buy EEG yet.

Start with EMG.

### Hardware

My first purchase would be the current **MyoWare 2 Muscle Sensor**.

It provides:

- raw EMG;
- rectified signal;
- envelope output;
- adjustable gain;
- microcontroller-friendly operation.

The current sensor is about $43, electrodes around $10/10, and its battery power shield around $16. [S57, S58, S59]

For early experiments:

```text
MyoWare
   ↓
microcontroller
   ↓
Bluetooth/serial
   ↓
Python SignalService
```

Follow the manufacturer's electrode-placement documentation rather than improvising around sensitive locations.

Also, keep human-connected biosignal electronics battery-powered/appropriately isolated. For example, OpenBCI explicitly specifies battery-only operation for its Cyton hardware. [S60]

---

## Experiment 2A — binary intention

Forget walking initially.

Pick something ridiculously simple:

> flex → virtual hand closes.

Use one large superficial muscle.

First test the MyoWare **envelope** rather than raw EMG.

Record:

```text
REST
FLEX
REST
FLEX
...
```

Maybe 30–50 repetitions.

Plot the signal.

You'll probably see something conceptually like:

```text
EMG
1.0 |                 ███
    |      ██         ███
0.5 |      ██   ██    ███
    |      ██   ██    ███
0.0 |████████████████████████
      rest flex rest flex
```

Now calculate something like:

$$
T=\mu_{\text{rest}}+k\sigma_{\text{rest}}
$$

and initially classify:

$$
x>T \Rightarrow \text{intent active}.
$$

Don't use machine learning simply because you can.

First determine whether the signal is separable.

---

### Phase 2 engineering targets

These are engineering goals I'd choose, not biological laws:

- >95% reliable rest/flex detection;
- <1 false activation/minute during rest;
- <150 ms perceived command latency;
- recalibration <30 seconds;
- still works after removing and reattaching electrodes.

The last one matters enormously.

A classifier that's 99% accurate immediately after training but collapses tomorrow isn't useful.

---

## Experiment 2B — classify several intents

Add sensors progressively.

For example:

```text
sensor A → left arm activation
sensor B → right arm activation
sensor C → left leg activation
sensor D → right leg activation
```

Your model becomes:

$$
f(X_t)
\rightarrow
P(\text{intent}_i)
$$

where intents might include:

```text
rest
left-hand action
right-hand action
walking pulse L
walking pulse R
```

Start with simple features:

- mean absolute value;
- RMS;
- variance;
- waveform length;
- envelope peak.

Then try:

- logistic regression;
- LDA;
- random forest.

Only later consider deep learning.

Small biosignal datasets frequently do **not** justify giant neural networks.

---

## Phase 3 — motionless locomotion

This is where the project starts becoming unusual.

### Goal

Determine whether you can create:

$$
\text{walking intention}
\rightarrow
\text{avatar locomotion}
$$

while producing almost no physical displacement.

You're replacing:

```text
thumbstick
```

with:

```text
body-derived intent.
```

---

## Experiment 3A — EMG walking

Recline comfortably.

Position EMG sensors according to manufacturer guidance on appropriate leg muscles.

Use very small alternating contractions:

```text
left
right
left
right
```

Interpret cadence as walking.

For example:

$$
v = kf_{\text{step-intent}}
$$

where $f$ is detected virtual step frequency.

The avatar animation handles actual biomechanics.

Your real legs barely move.

---

### Important architectural principle

Never send raw biosignals directly into gameplay logic.

Create:

```text
struct IntentFrame {
    timestamp;
    walk_forward;
    turn_left;
    turn_right;
    grab_left;
    grab_right;
    action_strength;
    confidence;
}
```

Then:

```text
EMG
↓
Intent Decoder
↓
IntentFrame
↓
Unreal
```

Later:

```text
EEG + EMG + eye tracking
↓
Intent Decoder
↓
same IntentFrame
```

This is what prevents Phase 5 from requiring a rewrite.

---

## Experiment 3B — compare against joystick locomotion

Create a fixed course.

Example:

```text
start
↓
walk 10 m
↓
turn left
↓
walk around obstacle
↓
approach table
↓
stop inside circle
```

Run:

### Condition A
Joystick locomotion.

### Condition B
EMG locomotion.

Measure:

| Metric | Why |
|---|---|
| completion time | usability |
| overshoot | control precision |
| unintended movement | false activation |
| latency | responsiveness |
| VEQ agency | embodiment |
| VEQ ownership | embodiment |
| sickness/discomfort | viability |

Do **not** expect EMG to beat a joystick initially.

What you're really looking for is something more interesting:

> Does biologically derived locomotion increase *agency* enough to compensate for worse raw control performance?

If yes, you're learning something genuinely relevant to full-dive interfaces.

---

## Experiment 3C — reduce physical activation

Once locomotion works:

Measure the EMG amplitude necessary to trigger movement.

Gradually train your classifier on weaker contractions.

Your experimental variable becomes:

$$
R =
\frac{\text{EMG activity during virtual locomotion}}
{\text{EMG activity during actual locomotion}}
$$

Try to reduce $R$ while maintaining:

- control accuracy;
- low false positives;
- strong agency.

That is an extremely interesting metric for this entire project.

---

## Phase 4 — turn the chair into a sensory pod

Only now would I start building something pod-like.

Not a sealed capsule.

Start with:

> **sensory recliner v1**

### Hardware hierarchy

I'd add modalities in this order.

#### 1. Localized vibration

Already built during Phase 1.

Increase from 2–4 zones toward perhaps:

```text
left/right shoulder
left/right forearm
left/right torso
left/right thigh
```

---

#### 2. Whole-body low-frequency impact

A bass shaker is excellent here.

The Dayton BST-1 is a 50 W, 4-ohm tactile transducer intended specifically for mounting to chairs, couches and simulation rigs. Current pricing is roughly $55–78 depending on retailer. [S61]

Mount it structurally to the chair.

Now events like:

```text
footsteps
explosion
large weapon hit
vehicle engine
landing
```

can produce body-scale mechanical feedback.

---

#### 3. Directional airflow

A few computer/blower fans:

```text
front
left
right
possibly overhead
```

Control them through a microcontroller.

Then:

```text
avatar velocity ↑
→ airflow ↑
```

or:

```text
wind from left
→ left fan
```

This is cheap but potentially very effective because it adds another sensory cue that agrees with vision.

---

#### 4. Commercial haptic vest — optional

You could also test a commercial system instead of DIYing everything.

bHaptics currently lists:

- TactSuit Air: about $320;
- TactSuit Pro: about $565;
- TactSleeve: about $225. [S62]

For **research**, however, I prefer your custom motors initially because you control:

- location;
- timing;
- amplitude;
- firmware;
- experimental condition.

Commercial hardware is excellent for quickly answering:

> "Does dense body haptics help?"

but worse for answering:

> "Which exact sensory parameter caused the improvement?"

---

## Experiment 4A — sensory ablation study

Now test:

```text
VR + audio
```

vs.

```text
VR + audio + localized haptics
```

vs.

```text
VR + audio + haptics + chair vibration
```

vs.

```text
VR + audio + haptics + chair vibration + airflow
```

Randomize the order.

Same virtual scenario.

Maybe:

> walk through a windy forest → pick up sword → strike shield → explosion in distance.

Measure VEQ after each condition.

This tells you which physical systems are actually purchasing immersion.

That matters because your eventual pod shouldn't become:

> $15,000 of hardware whose effects your brain barely notices.

---

## Experiment 4B — virtual weapon embodiment

This would be one of my major milestones.

Make a sword interaction entirely through subtle intent.

Sequence:

```text
look at sword
↓
EMG grab intent
↓
virtual hand grabs sword
↓
EMG arm intent
↓
game predicts swing
↓
sword hits target
↓
forearm haptic
+ bass-shaker impact
+ spatial audio
```

You haven't reproduced a sword's actual weight.

But you're testing whether sensory coherence convinces the brain that the virtual action occurred.

That's essentially the engineering shortcut we're betting on.

---

## What I would NOT build into the pod yet

No:

- body restraints;
- motorized limb immobilization;
- electrical stimulation through the head;
- DIY nerve stimulation;
- galvanic vestibular stimulation;
- attempts to induce sleep;
- attempts to inhibit movement neurologically.

Your chair should **support** your body rather than trap it.

Mechanical safety should always fail open.

---

## Phase 5 — introduce EEG

Only buy EEG equipment once Phases 1–4 are producing useful results.

At this stage the question becomes:

> **Can EEG detect motor intention/imagery early enough or reliably enough to replace some EMG commands?**

Not:

> "Can EEG read my thoughts?"

That framing matters.

Motor-imagery EEG commonly focuses on sensorimotor $\mu$ and $\beta$ rhythms. Reviews place useful activity roughly around $\mu \approx 8–15$ Hz and $\beta \approx16–31$ Hz, with hand-related motor-imagery information commonly measured around C3/C4 and foot imagery around Cz. [S63, S64]

---

## EEG hardware options

### Budget research option — OpenBCI Ganglion

- 4 channels;
- 200 Hz;
- BLE;
- open platform;
- currently $624.99 before electrodes/headgear. [S65, S66]

For a first motor-imagery experiment, four channels can be enough to explore sensorimotor locations.

This would probably be **my choice for you first**.

### More flexible — OpenBCI Cyton

- 8 channels;
- 250 Hz;
- $1,249 board alone. [S39, S67]

Better if you become seriously interested in EEG research.

### Easier consumer system — Neurosity Crown

Currently:

- 8 channels;
- 256 Hz;
- C3, C4, CP3 and CP4 among its sensor locations;
- dry electrodes;
- SDK access;
- $1,499. [S38, S68, S69]

That's convenient for your exact motor-interface interests.

But I would prefer OpenBCI if the goal evolves into serious instrumentation research because you get more freedom over electrode placement and the hardware stack.

BrainFlow already supports OpenBCI, Neurosity, Muse and many other boards through a common API. [S41]

---

## Experiment 5A — can you see motor activity at all?

Don't immediately classify imagined movement.

Start with:

```text
REST
↓
actually move right hand
↓
REST
↓
actually move left hand
```

Record:

- C3;
- C4;
- perhaps Cz;
- other nearby channels available.

Confirm that you can observe reproducible sensorimotor changes.

This validates:

- electrodes;
- placement;
- impedance/contact;
- acquisition;
- timestamps;
- preprocessing.

Only then move to imagery.

---

## Experiment 5B — motor imagery

Use two classes first:

```text
LEFT HAND IMAGERY
RIGHT HAND IMAGERY
```

A trial might be:

```text
2 s rest
↓
visual cue
↓
3–4 s motor imagery
↓
rest
```

Collect perhaps 20–40 clean trials per class initially.

Do several runs.

Do not continuously stare at accuracy and unconsciously change the experiment.

---

### Basic signal pipeline

A reasonable starting point is:

```text
EEG
↓
60 Hz interference rejection
↓
sensorimotor band extraction
↓
epoch around cue
↓
spatial features
↓
CSP
↓
LDA
↓
left / right
```

CSP + LDA remains a good baseline for motor-imagery work.

The important point:

#### Do not randomly split adjacent EEG windows into train/test sets.

That can create massive information leakage because neighboring windows are strongly correlated.

Instead:

```text
runs 1–3 → training
run 4 → validation
```

Eventually:

```text
Monday → training
Tuesday → held-out evaluation
```

That tells you whether the BCI actually generalizes.

---

## Phase 5 success criterion

For two classes:

$$
P_{\text{chance}} = 0.5.
$$

Don't obsess over some magic accuracy number.

I'd advance when you can get **repeatably above chance on held-out runs/sessions**, with enough accuracy that the system feels intentional rather than random.

Something like sustained 70%+ on genuinely held-out two-class trials would already be interesting for a hobbyist system.

---

## Experiment 5C — put EEG into VR

Do **not** make EEG control walking first.

Use something forgiving.

For example:

```text
LEFT imagery → left portal highlighted
RIGHT imagery → right portal highlighted
```

or:

```text
left imagery → virtual left hand glows
right imagery → virtual right hand glows
```

Then:

```text
confidence > threshold
for N milliseconds
↓
confirm action
```

You'll immediately learn how different:

> offline classifier accuracy

is from:

> usable real-time BCI.

---

## Phase 6 — the interesting architecture: EEG + EMG + context

This is where I think your eventual system could become genuinely impressive.

Don't ask EEG to do everything.

Suppose:

```text
gaze     → sword
EEG      → movement preparation
EMG      → tiny right-arm activation
context  → sword is grabbable
```

Then:

$$
P(\text{grab sword}\mid
EEG,EMG,gaze,context)
$$

may become extremely high.

The system can infer:

> user intends to pick up sword.

Then Unreal supplies the missing biomechanics.

That is far more practical than expecting EEG to continuously decode every joint.

---

## The experiment-management system matters almost as much as the pod

For every session, automatically save:

```yaml
experiment: emg_locomotion_003
hardware:
  headset: Quest3
  emg: MyoWare2
  haptics: custom_v2

software:
  unreal_commit: 8ab34d2
  biosignal_commit: af917c1

condition:
  locomotion: emg
  haptics: true
  latency_ms: 25

metrics:
  completion_time_ms:
  false_activations:
  classification_accuracy:
  mean_latency_ms:

questionnaires:
  veq_ownership:
  veq_agency:
  veq_change:

notes:
```

Otherwise, six months from now you'll have:

> "Version 14 felt really good."

and no idea what changed.

---

## Metrics I'd track throughout the entire project

Create a dashboard around five categories.

### Control

$$
Accuracy,\ Precision,\ Recall,\ F_1
$$

plus:

- false activations/min;
- missed intents;
- activation latency.

### Signal quality

- SNR;
- baseline drift;
- signal amplitude;
- electrode reattachment stability.

### Immersion

- VEQ ownership;
- VEQ agency;
- body-schema change.

The VEQ specifically validates those three constructs. [S51]

### Performance

- task completion time;
- error count;
- trajectory error.

### Physical effort

This one is particularly interesting for your project.

Track:

$$
E=
\frac{\text{virtual task muscle activation}}
{\text{normal task muscle activation}}
$$

Your long-term objective becomes:

$$
E \rightarrow 0
$$

while:

$$
Agency \rightarrow 1
$$

conceptually.

That's effectively your **"partial-dive score."**

---

## What I would buy first

Assuming you already have a PC but no experimental hardware, I would *not* dump $2,000 into EEG.

Start with approximately:

| Priority | Item | Purpose |
|---|---|---|
| Required | VR headset if needed | visual world |
| Required | MyoWare EMG sensor | intent sensing |
| Required | electrodes + battery/power module | EMG |
| Required | ESP32 or equivalent | sensor/haptic controller |
| Required | 2–4 DRV2605L boards | haptics |
| Required | 4 vibration motors | spatial touch |
| Useful | reclining chair | body support |
| Phase 4 | Dayton BST-1 + amp | impact/whole-body feedback |
| Phase 4 | controllable fans | airflow |
| Phase 5 | OpenBCI Ganglion | EEG |

The initial custom electronics excluding VR could realistically be **well under a few hundred dollars**.

The EEG is where costs suddenly jump.



---

## The sequence I would actually follow

1. **Create the Unreal/OpenXR experiment lab.** Get the avatar, mirror, interactions, VEQ logging, LSL timestamps and experiment runner working.
2. **Build four-zone haptics.** Demonstrate that synchronized virtual touch produces measurably stronger ownership than delayed/mismatched feedback.
3. **Add one-channel EMG.** Reliably control one virtual hand action without buttons.
4. **Expand EMG into locomotion.** Walk through a virtual environment while physically reclining and barely moving.
5. **Quantify minimum required movement.** Train the system to respond to progressively weaker contractions.
6. **Build sensory recliner v1.** Add multiple haptic zones, bass transducer and airflow.
7. **Run modality-ablation experiments.** Determine which combinations actually improve embodiment.
8. **Build a small interaction scenario.** Sword pickup/combat would be ideal.
9. **Only then buy passive EEG.** Establish motor execution signals first, imagery second.
10. **Fuse EEG + EMG + VR context.** Begin replacing explicit muscular commands with inferred intent.

If you get through **#6** successfully, you'll already have something that is substantially different from normal consumer VR: a reclining system where you can inhabit and move a virtual body using subtle biological intent while receiving synchronized physical feedback.

And #5 may actually become the most interesting research thread of the entire project: systematically discovering **how close muscle activity can approach zero while subjective agency stays high**. That's a tractable experimental proxy for the eventual transition from physical input to brain-derived input.
