# 05 — Synthetic Physiology: The Mana Core

| | |
|---|---|
| **Source** | User question in the archived conversation (see [transcript](transcript.md)) |
| **Date of record** | 2026-10-05 |
| **Related** | [Roadmap](../roadmap.md) · [Metrics](../metrics.md) · experiments [101-mana-core-loop](../../experiments/101-mana-core-loop/), [102-tactile-flow-illusion](../../experiments/102-tactile-flow-illusion/), [103-embodied-vs-arbitrary-casting](../../experiments/103-embodied-vs-arbitrary-casting/), [104-learned-mana-sensation](../../experiments/104-learned-mana-sensation/), [105-mana-control-language](../../experiments/105-mana-control-language/) · next: [06 — Incantations](06-incantations-and-casting.md) |

**The question this answers:**

> Another extension of this, part of what makes VR so exciting is being able to do things that you may not be able to in real life. The best example I personally can think of for this is magic/power systems like mana. A lot of these depictions in media are heavily rooted in science and history. For example a lot of pieces of media have like the existence of a “mana core” that a user learns to sense, and then hone and pull out their innate magic. I think that being able to recreate a sensation like that could be incredibly cool. And I feel like with the right perspective of implantation, we could get a convincing adaptation of this, like where a user focuses their mind in and like contracts their core muscles where they sense their “mana core” and then “draw out that mana” through other muscle sensations (like I can pretend to do this by my breathing and a sensation of slight muscle flexes from my core into my arms with my minds intention)

## Summary given up front

Yes — this is probably one of the most compelling directions for the project, because you are no longer trying to imitate a real-world action. You are designing an **entirely new embodied control system** and teaching the brain that a repeatable internal sensation corresponds to a fictional resource like mana.

Yes. I think this could become one of the **most distinctive parts of the entire project**, because unlike walking or sword swinging, there is no real-world reference that the simulation has to perfectly reproduce.

You can invent a new bodily sense and then train the user's nervous system to associate it with a consistent virtual mechanic.

A good name for the research direction would be **synthetic interoception** or **virtual-organ embodiment**.

Interoception is your perception of internal bodily states—breathing, heartbeat, tension, fullness, etc. Breathing is particularly promising: experiments have shown that synchronizing a virtual body's breathing with a participant's real respiration measurably increases virtual body ownership and agency. [S70, S71]

So your instinct of:

> focus inward → engage core → breathe → feel something move outward through the arms

is actually a surprisingly sensible control vocabulary.

## I would literally create a "mana organ"

Not anatomically, obviously. Perceptually.

Choose a fixed point inside the avatar:

```text
              chest
                │
                │
          ╭───────────╮
          │           │
          │     ●     │ ← virtual mana core
          │           │
          ╰───────────╯
               abdomen
```

Then make **everything involving mana consistently originate from that location**.

Initially, the user's real body receives a physical cue there—perhaps a small vibrotactile actuator against the abdomen/back—while VR provides:

- a faint internal glow visible through the avatar;
- a low spatial hum;
- subtle vibration;
- visual particles or distortion;
- synchronized avatar breathing.

Over time the brain learns:

$$
\text{that internal location}
\equiv
\text{mana source}.
$$

It isn't unprecedented for people to embody things that biologically don't exist. Experiments have demonstrated ownership and agency over **additional virtual limbs**, including a subjective sensation of having more arms after learning to control them. [S72]

That makes a fictional internal organ considerably less absurd as an HCI experiment than it initially sounds.

---

## The control language could be exceptionally cool

I'd make mana casting a multistage process rather than a button.

For example:

### 1. Sense

Relax your body and focus attention around the abdomen.

The system measures:

- respiration;
- abdominal EMG;
- perhaps eventually heart rate;
- general motion.

Nothing necessarily happens immediately.

Then a faint pulsing sensation appears at the "core."

Conceptually:

$$
M_\text{awareness}
=
f(\text{respiration},\text{core EMG},\text{stillness})
$$

The goal is to make the player think:

> I can feel my mana core.

---

### 2. Gather

Now deliberately engage the abdomen slightly while controlling your breathing.

Not a giant crunch.

Just enough activation for EMG to recognize:

$$
\text{rest}
\rightarrow
\text{intentional core activation}.
$$

The core begins to:

- vibrate more strongly;
- brighten;
- produce a deeper sound;
- perhaps visually expand slightly.

Your "mana pressure" increases.

Importantly, I'd **avoid making breath-holding or hyperventilation part of the mechanic**. Normal comfortable breathing should be enough.

---

### 3. Draw the mana out

This is where the illusion could become genuinely weird.

Put small haptic motors at perhaps:

```text
abdomen
   ↓
upper torso
   ↓
shoulder
   ↓
upper arm
   ↓
forearm
   ↓
hand
```

Don't activate them simultaneously.

Run something like:

```text
CORE     ███
TORSO       ███
SHOULDER       ███
ARM               ███
FOREARM              ███
HAND                    ███
       ─────────────────────→ time
```

Subjectively this could feel like **something traveling through your body**.

And there's unusually relevant research here.

With carefully timed vibration, people can perceive an illusory tactile stimulus **moving through locations where no actuator actually exists**. A 2025 experiment found that visual information strongly dominated these tactile-motion illusions; even just two physical actuators could generate convincing apparent trajectories when paired with appropriate visuals. [S73]

That's almost tailor-made for this application.

You don't necessarily need:

> 30 haptic motors running from stomach to fingertips.

You might discover:

> core actuator + arm actuator + visual "mana flow"

produces the perception:

> something moved from my core through my arm.

That's a fantastic experiment.

---

## Then associate that sensation with actual intent

Let's say you're drawing mana toward your right hand.

You could require:

```text
slight core contraction
       +
controlled inhale/exhale
       +
subtle right forearm activation
       +
attention/gaze toward right hand
```

Then calculate something like:

$$
D_R =
w_1E_\text{core}
+w_2E_\text{right arm}
+w_3R_\text{breathing}
+w_4G_\text{hand}.
$$

Where $D_R$ is **draw-right mana intent**.

Once the threshold is crossed:

```text
mana core
    ↓
torso sensation
    ↓
shoulder sensation
    ↓
arm sensation
    ↓
hand
```

and your virtual hand begins glowing.

You're essentially inventing a **biological gesture that doesn't naturally exist**.

That's much more interesting to me than mapping:

> flex bicep → press X.

---

## Magic schools could actually have different "body languages"

Eventually you could exploit this dramatically.

A fire spell might involve:

> core → dominant arm → palm

and feel fast, warm-looking visually, noisy and aggressive.

A healing spell:

> core → chest → both arms → palms

with slower rhythmic haptics.

Telekinesis:

> core activation → shoulder/arm intention without actual movement → gaze target.

Shield:

> abrupt core brace + bilateral arm activation.

Lightning:

> fast core impulse → narrow rapid traveling sensation → fingertips.

Large spell:

> sustained low-level core activation → mana accumulation → directional release.

You'd effectively be designing a **somatic grammar for magic**.

Instead of:

```text
A = fireball
B = shield
X = heal
```

you get:

```text
internal body state
      ↓
intent
      ↓
mana routing
      ↓
spell construction
```

That could feel *far* closer to fantasy fiction than normal game controls.

---

## I would make mana continuous rather than binary

You shouldn't simply detect:

$$
\text{core flex}=\text{true}.
$$

Measure intensity.

For example:

$$
C =
\frac{EMG-\mu_\text{rest}}
{\mu_\text{max}-\mu_\text{rest}}
$$

giving:

$$
C\in[0,1].
$$

Then perhaps:

$$
\frac{dM}{dt}=kC.
$$

Very light activation:

> slowly gather mana.

Stronger controlled activation:

> rapidly draw mana.

Poor control / excessive tension:

> inefficient.

That gives the player something they can genuinely **learn to become better at**.

Someone experienced might eventually generate the same in-game mana flow with extremely subtle physical activation.

That's exactly aligned with the overall project.

---

## Training could produce something surprisingly close to "sensing mana"

And this is where it becomes a legitimate research question rather than merely a game mechanic.

Initially:

$$
\text{muscle activation}
\rightarrow
\text{computer detects it}
\rightarrow
\text{haptic sensation}.
$$

After hundreds of repetitions, you might develop an extremely strong expectation:

$$
\text{intention}
\rightarrow
\text{expected sensation}.
$$

There is evidence that VR can produce **phantom touch sensations even with no physical tactile stimulation**. In one experiment, 89% of participants reported some sensation when seeing their virtual body being touched despite receiving no corresponding physical stimulus. People commonly described tingling, static or wind-like sensations. [S74]

That doesn't mean we can guarantee you'll hallucinate mana.

But it raises a fantastic experimental possibility.

Suppose you train:

```text
core activation
↓
traveling abdomen → arm haptic
↓
mana appears
```

for weeks.

Then occasionally secretly run:

```text
core activation
↓
visual mana movement
↓
NO haptic motors
```

and ask:

> Did you feel the mana move?

That is a genuinely interesting experiment.

If the answer changes with training, you're investigating whether repeated multisensory learning can create a **learned synthetic bodily sensation**.

---

## I'd add this as an entire research branch

Your roadmap now becomes two parallel goals:

```text
                  PARTIAL DIVE
                       │
             ┌─────────┴─────────┐
             │                   │
       Virtual Body        Synthetic Body
        Embodiment             Senses
             │                   │
      movement intent       mana / powers
      virtual touch         artificial organs
      locomotion            new control grammar
      body ownership        learned interoception
```

And I'd actually start the **mana experiment very early**, around the EMG/haptics stage rather than waiting for EEG.

### Mana Experiment V0

You could build this with:

- 1 abdominal EMG sensor;
- 2–4 small vibration motors;
- ESP32;
- VR headset;
- Unreal;
- optionally a simple respiration/stretch sensor.

Then implement only:

> **Sense → Gather → Draw → Release**

#### Sense
Remain relaxed. A subtle virtual/haptic core pulse becomes detectable.

#### Gather
Slight abdominal activation increases virtual mana.

#### Draw
Forearm activation routes it toward one hand.

#### Release
A distinct small hand/forearm gesture casts.

Something like:

```text
               RELAX
                 │
                 ▼
          ┌─────────────┐
          │ Sense Core  │
          └──────┬──────┘
                 │
            core EMG ↑
                 ▼
          ┌─────────────┐
          │ Gather Mana │
          └──────┬──────┘
                 │
       right forearm EMG ↑
                 ▼
          abdomen
             ●
             │
             ▼
          shoulder
             │
             ▼
           hand ✦
                 │
          release gesture
                 ▼
              FIREBALL
```

That alone would be enough to answer several interesting questions.

---

### The key experiments I'd run

I'd specifically test whether **embodied control beats arbitrary control**.

For example:

**Condition A:** Button press → fireball.

**Condition B:** Hand flex → fireball.

**Condition C:** core gather → tactile flow through arm → hand charge → release.

Then measure:

- agency;
- presence;
- perceived "power";
- physical effort;
- learning time;
- enjoyment;
- how strongly the user felt that the magic originated from their body.

I'd expect C to be slower than pressing a button.

That's fine.

The research question isn't:

> Which is the fastest UI?

It's:

> **Which makes you feel like you actually possess the fictional ability?**

That is a fundamentally different design objective.

And the existing embodiment research gives your idea more scientific grounding than I initially would have expected: respiration can influence virtual-body agency, people can embody anatomically impossible additions, vision can create motion between sparse tactile actuators, and virtual contact can sometimes generate sensation without physical contact at all. [S75, S76]

If this works well, "mana" wouldn't just be a HUD bar. It becomes a **new learned relationship between attention, breathing, muscle state, tactile perception and a virtual body**. That's about as close as current non-invasive technology can reasonably get to teaching someone to *use magic*.
