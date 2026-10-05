# 07 — Version Roadmap (V0–V6): Two Branches, One Platform

| | |
|---|---|
| **Source** | User question in the archived conversation (see [transcript](transcript.md)) |
| **Date of record** | 2026-10-05 |
| **Related** | [Roadmap](../roadmap.md) (operational version of this) · experiments [000-loop-latency](../../experiments/000-loop-latency/), 001–013, 101–107, [201-locomotion-plus-magic](../../experiments/201-locomotion-plus-magic/) |

**The question this answers:**

> Yeah this is what I am most excited to explore, so with this all in mind, does it still just fall into the same distinctive experiment paths as before, we focus on v0 with just getting walking by intent working, then v2 could be exploring this synthetic physiology with magic?

Yes, but I’d make one important adjustment: **don’t treat synthetic physiology as something that has to wait until locomotion is “finished.”** The two problems share the same underlying platform but test different hypotheses, and the mana system may actually be easier to make compelling than motionless walking.

I’d structure the research program as two branches growing from the same core.

```text
                    PARTIAL-DIVE PLATFORM
                            │
             biosignals → Intent Bus → Unreal
                            │
                 synchronized haptics/logging
                            │
              ┌─────────────┴─────────────┐
              │                           │
      EMBODIED CONTROL             SYNTHETIC PHYSIOLOGY
              │                           │
       walking / hands                mana core
       locomotion                     mana routing
       virtual body                   spell formation
       motor intent                   chanting
              │                           │
              └─────────────┬─────────────┘
                            │
                 FULL EMBODIED EXPERIENCE
```

The version sequence I'd actually use is:

## V0 — Prove the intent loop

This is your foundational prototype.

Don't even make "walking" the requirement yet. Prove:

$$
\text{subtle biological intent}
\rightarrow
\text{reliable detection}
\rightarrow
\text{immediate virtual response}
$$

For example:

- reclined in VR;
- forearm EMG;
- lightly contract muscle;
- virtual hand closes;
- release;
- hand opens.

Then prove the same architecture can produce a haptic response.

This validates the entire:

```text
body
↓
sensor
↓
classifier
↓
IntentFrame
↓
Unreal
↓
haptic feedback
↓
body
```

loop.

That's your actual **V0**.

---

## V1 — Motionless embodiment / locomotion

Now attack the original partial-dive problem.

Objectives:

- reclined avatar embodiment;
- subtle left/right leg activation;
- virtual walking;
- progressively reduce physical movement;
- measure agency;
- compare against joystick;
- add basic contact haptics.

The big research metric becomes something like:

$$
\frac{\text{physical movement required}}
{\text{virtual movement produced}}
\rightarrow 0
$$

while perceived agency stays high.

By the end of V1, ideally you can lie comfortably in the chair and:

> walk around, stop, turn, grab something and interact

without conventional controllers being your primary interface.

That is already a major milestone.

---

## V2 — Synthetic physiology V0

This is where I'd introduce the first mana system.

And I'd keep it extremely constrained.

Don't build fire/water/wind/earth yet.

Build **one fictional bodily phenomenon**:

> You have a mana core.

That's it.

You need to determine whether you can make someone experience a coherent loop of:

$$
\text{Sense}
\rightarrow
\text{Gather}
\rightarrow
\text{Route}
\rightarrow
\text{Release}.
$$

For example:

**Sense**

Relaxation + attention toward the abdomen.

A very subtle repeating haptic signal establishes the core location.

**Gather**

Slight abdominal EMG activation.

The virtual core brightens and the haptic pulse becomes stronger.

**Route**

Subtle right-arm activation.

Timed haptics move:

```text
abdomen → chest → shoulder → arm → hand
```

**Release**

Small intentional hand/forearm signal.

A ball of generic "mana" leaves the hand.

No element.

No damage system.

No enemies.

No skill tree.

Just:

> **Can I make this feel like something originated inside my body and traveled into my hand?**

That should be V2's central experiment.

---

## V2.1 — Does a mana core become learnable?

This is potentially the scientifically interesting part.

Train with it repeatedly.

You want to see whether:

```text
core activation
    ↓
expected internal sensation
    ↓
routing sensation
    ↓
hand
```

starts becoming one learned motor/sensory sequence rather than four separate game mechanics.

Then run blinded tests.

Sometimes:

```text
full haptics
```

Sometimes:

```text
reduced haptics
```

Sometimes:

```text
visual only
```

Don't tell yourself which.

Then record:

> Did you feel something move?

> Where did it start?

> Where did it end?

> How strong was it?

> Did it feel like an external vibration or something belonging to your virtual body?

That experiment is arguably more interesting than whether you can successfully shoot the mana ball.

---

## V3 — Magic becomes a control language

Once generic mana works, **then** introduce different behaviors.

Start with perhaps three operations:

```text
GATHER
ROUTE
SHAPE
```

Then:

```text
GATHER + ROUTE RIGHT + PROJECTILE
```

creates one effect.

While:

```text
GATHER + ROUTE BOTH ARMS + EXPAND
```

creates another.

Now the player isn't selecting spells.

They're manipulating a virtual physiological system.

This is where I'd first introduce something resembling schools.

Maybe just:

### Fire

Fast/high-intensity flow.

### Water

Slow/sustained flow.

### Force/neutral mana

Baseline.

You don't need ten schools yet.

You want to test:

> Can users learn **different internal control patterns** and reliably associate them with different virtual phenomena?

---

## V4 — Incantations

Then add voice.

The reason I wouldn't add chants immediately is that they introduce another variable.

First determine whether:

> synthetic physiology alone works.

Then ask:

> Does structured speech make synthetic physiology easier to learn/control?

That's the interesting comparison.

You could test:

### Condition A
Mana casting without speech.

### Condition B
Arbitrary phrase.

### Condition C
Structured chant whose phrases correspond to casting stages.

I'd strongly expect C to be the most interesting.

Example:

```text
phrase 1 → gather
phrase 2 → route
phrase 3 → shape
phrase 4 → release
```

Eventually you can investigate whether users need fewer verbal cues as their bodily skill improves.

That's where the *Mushoku Tensei*-style progression becomes meaningful.

---

## V5 — Skill acquisition / silent casting

This is where it stops being just an interface experiment and starts becoming something quite unusual.

Your system tracks actual competency:

$$
\text{stability}
$$

$$
\text{accuracy}
$$

$$
\text{required EMG amplitude}
$$

$$
\text{casting latency}
$$

$$
\text{dependence on verbal cues}.
$$

A beginner might need:

```text
full body process
+
full chant
+
strong haptic guidance
+
obvious visual indicators
```

An experienced user might achieve the same spell with:

```text
tiny core activation
+
tiny arm signal
+
no chant
+
minimal visual guidance
```

That's when you could legitimately say the user has **learned the magic system** rather than merely unlocked abilities.

And I'd make this one of your core research metrics:

$$
S =
f(
\text{physical effort}^{-1},
\text{accuracy},
\text{agency},
\text{cue dependence}^{-1}
)
$$

where $S$ represents learned synthetic-physiology proficiency.

---

## V6 — Merge locomotion and magic

Only once both branches work separately would I really merge them.

Now you're:

- reclined;
- virtually walking through an environment;
- inhabiting an avatar;
- interacting through subtle motor intent;
- sensing a fictional mana core;
- routing mana through that same virtual body;
- chanting;
- casting while moving.

That's when the whole thing starts approaching the fantasy you're imagining.

Because the important perceptual transition becomes:

> I'm lying in a chair controlling a VR character.

to:

> **I'm in this body, walking through this environment, and this body has abilities my physical body doesn't have.**

That's ultimately the experience you're researching.

---

## So I'd slightly revise your numbering

I wouldn't make:

> V0 = walking  
> V2 = magic.

I'd make:

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

And you can absolutely start thinking about V2 while building V0/V1.

The nice thing is that practically everything you buy for the early system—EMG, haptic motors, ESP32s, Unreal integration, logging, experiment framework—gets reused for the mana research. You're not building two projects. **You're building one experimental nervous-system interface, and locomotion and magic are simply two different applications of it.**
