# 03 — Doing Meaningful Non-Invasive Research Yourself

| | |
|---|---|
| **Source** | User question in the archived conversation (see [transcript](transcript.md)) |
| **Date of record** | 2026-10-05 |
| **Related** | [Research protocol](../research-protocol.md) · [Metrics](../metrics.md) · [Safety](../safety.md) · [Hardware](../hardware.md) · experiments [001-baseline-embodiment](../../experiments/001-baseline-embodiment/)–[012-eeg-in-vr](../../experiments/012-eeg-in-vr/) · next: [04 — Program guide](04-research-program-guide.md) |

**The question this answers:**

> I don’t have the capability to build something like this currently, but I think it would be cool to steadily work towards my own experiment for a pod like this, I don’t have like infinite money so it could be hard, is there any way I could do meaningful research and physical tests and stuff of my own for this type of experience? I obviously will not be injecting any sort of chip in my brain though

## Summary given up front

Yes. You could do genuinely useful work without touching an implant at all. The most productive path would be to treat this as a **human-computer interaction / neuroergonomics project**, not a “build NerveGear” project: quantify how far you can push immersion using noninvasive intent sensing, body support, haptics, and sensory consistency.

The research also points to a useful budget strategy: **don’t start with EEG.** EEG motor imagery is scientifically interesting but noisy and calibration-heavy; surface EMG gives you a much cleaner “intended movement” signal for tens of dollars per channel and lets you validate the interaction model first. Meta’s current Neural Band is built around exactly that principle—surface EMG plus machine learning to infer subtle intended actions. 

Once you know the interaction actually feels compelling, *then* EEG becomes worth the money.

Yes. In fact, I think this is a **very good garage-research project** precisely because you do *not* need to solve full-dive VR to contribute something meaningful.

The productive framing would be:

> **How close can I get to “the virtual body replaces the physical body” using only safe, non-invasive technology?**

That breaks the impossible problem into testable engineering questions. Current research already shows that synchronized visual, motor, and tactile information can produce surprisingly strong ownership over a virtual body, and that breaking those temporal relationships weakens the illusion. [S28, S29]

I would build toward a **reclined neural-interface VR pod**, one subsystem at a time.

## The first prototype I'd actually build

Don't build the pod first.

Build this:

```text
                  VR HEADSET
              vision + spatial audio
                       │
                       ▼
              ┌─────────────────┐
              │   Virtual Body  │
              │ Unity / Unreal  │
              └───────┬─────────┘
                      │
          ┌───────────┴───────────┐
          ▼                       ▼
       INPUT                    OUTPUT
    EMG / IMU              vibration / force
    eye tracking            airflow / audio
    later EEG               body pressure
          │                       │
          └──────────► YOU ◄──────┘
                  reclined
```

Your first research objective would be very specific:

> **Can I comfortably lie nearly motionless while controlling a first-person virtual body using tiny muscle activations rather than actual limb movement?**

That would already teach you a huge amount about the full-dive interaction problem.

---

## 1. Start with EMG instead of EEG

This is probably the single highest-value decision.

EEG measures brain activity. That's exciting, but noninvasive EEG has terrible signal-to-noise compared with implanted BCIs.

Surface electromyography measures the tiny electrical signals driving muscles.

So instead of requiring:

> think about moving hand

you initially require:

> initiate an extremely small hand movement / muscle contraction

without actually swinging your arm around.

Modern EMG interfaces can recognize extremely subtle gestures. Meta's current Neural Band uses surface EMG at the wrist, machine learning, and IMU data to infer user actions with millisecond-scale response. [S30]

You can experiment with the same fundamental concept extremely cheaply.

SparkFun currently sells individual MyoWare EMG sensors for about **$43** and a wireless kit around **$125**. [S31, S32]

Put sensors on something like your:

```text
forearm ───── grip / release
biceps ────── arm action
quadriceps ── walk
calf ──────── alternate locomotion signal
```

Then train a classifier:

$$
f(\text{EMG}_{1..n}) \rightarrow
\{\text{walk},\text{grab},\text{attack},\text{rest}\}
$$

You could begin with only 2–4 commands.

Later:

$$
f(\text{EMG time series})
\rightarrow
(\text{action},\text{intensity},\text{direction})
$$

And because you're a software developer, **this is where you could contribute meaningfully almost immediately**. The hardware isn't especially exotic; the interesting work becomes filtering, classification, latency, calibration and mapping intention into coherent avatar motion.

---

## 2. Build a "motionless locomotion" experiment

This is the experiment I'd be most interested in.

Sit or lie in a recliner.

In VR you're standing.

Instead of moving a joystick:

### Experiment A
Very lightly tense your left/right leg alternately.

Your avatar walks.

### Experiment B
Eventually detect simultaneous walking-related activation patterns and translate their intensity into speed.

### Experiment C
Compare it against joystick locomotion.

Measure:

- perceived agency;
- immersion;
- cybersickness;
- reaction time;
- control accuracy;
- how quickly the control becomes automatic.

What you're investigating is whether:

$$
\text{motor intention}
\rightarrow
\text{virtual movement}
$$

can eventually become perceptually similar to:

$$
\text{motor intention}
\rightarrow
\text{physical movement}.
$$

That's a legitimate HCI/neuroscience question.

---

## 3. Then attack **body ownership**

This may actually produce the most dramatic results.

There is substantial experimental evidence that people can begin treating a virtual body as their own when first-person visual information and sensory information line up properly. Synchronous visual/tactile stimulation and synchronous avatar movement strengthen ownership and agency. [S33, S34]

So build something simple.

You see your virtual forearm.

Attach a little vibration motor to your real forearm.

A virtual object touches:

```text
virtual forearm
      ↓
game detects collision
      ↓
20 ms later
      ↓
real vibration motor fires
```

Now compare it against:

```text
virtual touch
      ↓
300 ms delay
      ↓
physical vibration
```

Ask yourself afterward:

> Did it feel like the virtual arm was being touched?

You can make this scientific rather than anecdotal.

Run randomized conditions:

| Condition | Visual touch | Physical touch |
|---|---|---|
| A | synchronized | synchronized |
| B | synchronized | +250 ms delay |
| C | synchronized | random location |
| D | none | physical only |

Then measure ownership.

Research already shows these timing/congruence effects are important. [S28, S35]

That's effectively you experimenting with the **sensory codec problem** without stimulating a single neuron directly.

---

## 4. Build increasingly rich physical feedback

This is where a pod becomes really useful.

And most of this hardware is inexpensive.

### Vibration

Small ERM/LRA motors.

Put them at:

```text
shoulders
forearms
hands
torso
thighs
feet
```

A game collision becomes a spatial tactile event.

---

### Low-frequency impact

Put a bass shaker/transducer in the chair.

Things like:

> explosion  
> footsteps  
> giant monster  
> vehicle engine  
> sword impact

can generate whole-body mechanical vibration.

The combination of:

**visual impact + sound + vibration**

is dramatically more convincing than any one component.

---

### Air

Fans are ridiculously useful for VR.

```text
walking      → mild airflow
running      → stronger airflow
falling      → strong airflow
wind spell   → directional airflow
```

Your brain gets another mutually consistent sensory cue.

---

### Pressure

Eventually, padded motorized surfaces could press on parts of the body.

Virtual person grabs your shoulder:

```text
collision
    ↓
shoulder actuator
    ↓
gentle physical pressure
```

No brain stimulation necessary.

I'd favor **pressure, vibration and airflow before temperature**. Thermal systems introduce burn/cold-injury failure modes surprisingly quickly.

---

## 5. Then try an actual reclining "pod"

Not some giant sci-fi capsule yet.

Something like:

```text
                 HEADSET
                 ╱─────╲
              headphones
                  │
          ┌───────┴───────┐
          │   headrest    │
       ╭──┴───────────────┴──╮
       │   shoulder haptics   │
       │                      │
       │    reclining body    │
       │                      │
       │ arm supports         │
       │      ╲        ╱      │
       │       ╲ legs ╱       │
       ╰──────────────────────╯
```

A used recliner / zero-gravity chair + foam + modular hardware would be enough.

The critical design principle would be:

**support, not restraint.**

You don't want to immobilize yourself. You want it comfortable for your physical body to remain relaxed while the virtual body moves.

And there should always be an immediate physical way to exit the system.

---

## 6. EEG comes later

Once your EMG-based system works, *then* EEG becomes scientifically interesting.

Your experiment changes from:

> detect tiny muscle activation

to:

> can I detect motor imagery before measurable muscular execution?

That's getting substantially closer to actual BCI control.

There are consumer EEG devices, but hardware choice matters enormously.

For motor imagery, you're particularly interested in sensorimotor areas near locations such as:

$$
C3,\;Cz,\;C4
$$

under the standard EEG 10–20 system.

That's why I'd **not buy a Muse 2 specifically for this experiment**, even though it's inexpensive at roughly $250. Muse 2's normal EEG locations are TP9, AF7, AF8 and TP10 — mostly not over the motor cortex. [S36, S37]

Something like the Neurosity Crown is much more interesting for this particular research question because its eight sensors include **C3, C4, CP3 and CP4**, although it's currently around $1,499. [S38]

OpenBCI gives you even more flexibility, but an 8-channel Cyton alone currently costs about $1,249, with a complete Ultracortex setup around $2,999. [S39, S40]

So:

**don't spend that money yet.**

Prove the interaction concept using $50–200 worth of EMG hardware.

---

## 7. There is a nice software ecosystem for this

You wouldn't have to invent the biosignal infrastructure yourself.

BrainFlow currently supports a large collection of EEG/biosignal boards including OpenBCI, Muse, Neurosity, PiEEG and others, giving you a common API for acquiring and processing signals. [S41, S36]

You could structure the software as:

```text
Sensors
   ↓
BrainFlow / serial
   ↓
Signal preprocessing
   ↓
Feature extraction
   ↓
ML intent classifier
   ↓
Intent bus
   ↓
Unreal / Unity
   ↓
Avatar controller
   ↓
Haptic controller
```

That `Intent Bus` abstraction would be especially useful.

The game wouldn't care whether:

```text
WalkIntent = 0.82
```

came from:

- joystick;
- EMG;
- EEG;
- eye tracking;
- some future BCI.

That's exactly how I would architect an experimental platform for this.

---

## 8. Eventually combine EMG + EEG

This is probably much more realistic than EEG alone.

Say you want:

> grab sword.

You collect:

$$
X =
[
EEG,
EMG,
IMU,
eye\ gaze,
head\ orientation
]
$$

and predict:

$$
P(\text{grab sword}\mid X)
$$

Maybe EEG gives a weak signal that motor planning has started.

EMG provides a stronger indication of what movement is beginning.

Eye tracking tells you which sword you're looking at.

The virtual environment tells you what interactions are possible.

Now the system may confidently infer:

> 97% probability: user intends to grab sword.

You don't need telepathy.

You need **sensor fusion**.

That's a much more tractable engineering problem.

---

## 9. You can measure whether you're actually making progress

This is important.

Don't make the goal:

> feels cool.

Measure things.

For every input method track:

$$
\text{accuracy}
$$

$$
\text{false activations/minute}
$$

$$
\text{command latency}
$$

$$
\text{information transfer rate}
$$

$$
\text{calibration time}
$$

And for immersion:

- sense of agency;
- body ownership;
- presence;
- cybersickness.

The Simulator Sickness Questionnaire (SSQ) remains one of the standard VR sickness measures, while questionnaires such as IPQ measure spatial presence, involvement and experienced realism. [S42, S43]

That makes your work much more interesting because you can say:

> EMG locomotion increased embodiment score by 23% relative to joystick locomotion while adding 74 ms median input latency.

rather than:

> this version felt better.

---

## 10. One boundary I'd put around the project

I would happily experiment on myself with:

- EEG **recording**;
- EMG recording;
- eye tracking;
- IMUs;
- mechanical haptics;
- vibration;
- audio;
- airflow;
- body-position illusions;
- VR sensory synchronization.

I would **not DIY electrical brain stimulation, nerve stimulation, or galvanic vestibular stimulation** as the next step.

Those are real research techniques, but once you're deliberately putting current through the head/nervous system, you've moved into a substantially different safety category.

There's already a gigantic amount of unexplored territory on the **read-only + physical-feedback** side before you'd gain much from doing that.

PiEEG's own open-source documentation, for example, explicitly warns that its EEG hardware is not a certified medical device and requires complete battery isolation from mains when attached to a person. [S44]

That's the kind of electrical-safety discipline you have to start thinking about with biopotential hardware at all.

---

## A realistic personal roadmap

If I were doing this on a normal personal budget, I'd make the first five milestones:

1. **VR body ownership demo** — reclined first-person avatar with synchronized vibration.
2. **EMG locomotion** — walk around VR while your legs remain essentially stationary.
3. **EMG hands/combat** — infer grab/release/attack from subtle muscle activation and use AI/IK to complete the motion.
4. **Prototype sensory pod** — modular recliner with localized vibration, bass transducers, fans and pressure feedback.
5. **EEG experiment** — only after everything else works, investigate whether motor imagery signals can substitute for some EMG commands.

You could probably get through **#1–#3 for hundreds of dollars rather than tens of thousands**, particularly if you already own a PC and VR headset.

And the most interesting research question wouldn't even be *"can my EEG control a character?"*

It would be:

> **How little physical movement can a human make while still experiencing strong agency and ownership over a freely moving virtual body?**

That is directly relevant to the eventual full-dive problem, experimentally measurable, currently achievable, and doesn't require doing anything invasive.

If this became a long-term hobby/research project, you could genuinely spend years progressively moving the boundary from **physical control → subtle muscular intent → multimodal biosignal intent → actual brain-derived intent**, while keeping the same VR/pod platform underneath it.
