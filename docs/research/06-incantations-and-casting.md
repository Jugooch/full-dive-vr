# 06 — Incantations, Casting, and Magic Schools

| | |
|---|---|
| **Source** | User question in the archived conversation (see [transcript](transcript.md)) |
| **Date of record** | 2026-10-05 |
| **Related** | [Architecture](../architecture.md) (voice/chant pipeline) · experiments [105-mana-control-language](../../experiments/105-mana-control-language/), [106-incantations](../../experiments/106-incantations/), [107-silent-casting-proficiency](../../experiments/107-silent-casting-proficiency/) · next: [07 — Version roadmap](07-version-roadmap.md) |

**The question this answers:**

> I think that a fleshed out version of this could also include “chants” or something similar where a user is able to draw out their mana and “use” it like we described in v0 with all those different schools, but imagine there being voice recognition as well where predetermined spell chants can increase the power and help power up the “spells” users can use. For example Mushoku Tensei does this well

Yes — and I think chants actually solve a design problem that the mana system otherwise has: they give the user a **structured cognitive scaffold for learning an imaginary bodily process**.

*Mushoku Tensei* is a particularly useful model because its magic system treats incantations as an activation aid, while skilled casters can eventually manipulate mana directly and omit or shorten them. [S77, S78] That translates extremely well into an actual VR training system.

I would not implement chants as simply:

> say correct phrase → +50% fireball damage.

I'd make the chant **part of the process of forming the spell**.

## A better model: casting as a sequence

Imagine every spell internally consists of:

$$
\text{Sense}
\rightarrow
\text{Gather}
\rightarrow
\text{Route}
\rightarrow
\text{Shape}
\rightarrow
\text{Charge}
\rightarrow
\text{Release}
$$

Your body handles some parts.

Your voice handles others.

Your attention/gaze handles others.

For a simple fire spell:

```text
1. SENSE
   relax + become aware of "mana core"

2. GATHER
   slight abdominal activation
   controlled breathing
   core haptic intensity rises

3. ROUTE
   subtle right-arm EMG
   haptic sensation travels core → shoulder → forearm → hand

4. SHAPE
   begin incantation
   spell particles begin forming

5. CHARGE
   continue chant
   maintain controlled mana flow
   spell grows in power/stability

6. RELEASE
   final word + hand intention
   spell fires
```

Now it feels like you're **performing magic**, rather than entering a voice command.

---

## Chants become a magic programming language

This could be much deeper than memorizing arbitrary phrases.

Each piece of the incantation can represent something in the spell model.

Conceptually:

```text
[Element]
    ↓
[Behavior]
    ↓
[Form]
    ↓
[Magnitude]
    ↓
[Release]
```

For example, an original fictional incantation might conceptually mean:

> Fire → gather → compress → projectile → release.

Another might mean:

> Water → gather → surround → rotate → barrier.

Internally the game sees something like:

```text
ManaSpell {
    element: FIRE
    form: PROJECTILE
    behavior: COMPRESSED
    magnitude: 0.72
    target: GAZE_TARGET
}
```

The spoken chant is therefore almost a **high-level language for manipulating the mana system**.

That's interesting because eventually you could let experienced players understand what the components mean rather than merely memorize entire spells.

---

## And the physical mana manipulation determines whether the chant succeeds

This is the part I'd emphasize heavily.

Suppose the player chants perfectly but isn't gathering mana.

The spell might form visually but collapse:

```text
Voice:      █████████████████  100%
Mana flow:  ██                 12%

             → spell fizzles
```

Conversely:

```text
Voice:      ███████████████    91%
Mana flow:  █████████████      83%
Control:    ███████████████    94%

             → successful cast
```

You could define:

$$
Q=
w_mM+w_vV+w_cC+w_fF
$$

where:

- $M$ = mana supplied,
- $V$ = chant accuracy,
- $C$ = physical control/stability,
- $F$ = spell-form accuracy.

But I'd avoid literally displaying those percentages during normal play.

The player should **feel** them.

Bad mana routing:

> unstable vibration.

Too much power:

> core becomes aggressive and noisy.

Bad chant:

> magic formation becomes distorted.

Correct formation:

> sensation becomes smooth and coherent.

That teaches the system somatically instead of through HUD bars.

---

## Chants could actually *guide* the physical process

This is perhaps the coolest part.

Give each phrase a physiological purpose.

For example:

```text
Phrase 1
"Awaken..."
      ↓
slow inhale
      ↓
core becomes perceptible

Phrase 2
"Gather..."
      ↓
core contraction
      ↓
mana accumulates

Phrase 3
"Flow..."
      ↓
arm activation
      ↓
haptic flow toward hand

Phrase 4
"Take form..."
      ↓
hold stable activation
      ↓
spell forms

Final phrase
"Release."
      ↓
release gesture
      ↓
CAST
```

After hundreds of casts, the words become associated with the corresponding internal states.

That's essentially classical sensorimotor learning.

Eventually, just beginning the chant may make your brain anticipate:

> core sensation → arm flow → hand charge.

And that's exactly the conditioning you'd want if the eventual objective is to make the fictional magic system feel natural.

---

## This gives you an incredible progression system

This is where I'd borrow the **principle** from *Mushoku Tensei*, because the progression maps beautifully to actual human skill acquisition.

### Beginner — Full incantation

The player requires:

```text
mana preparation
+
full chant
+
physical routing
+
release gesture
```

A spell may take 5–10 seconds.

The chant acts like training wheels.

---

### Intermediate — Shortened casting

Once proficiency is high:

```text
"Gather flame,
take form,
release."
```

instead of the entire incantation.

Why?

Because the system has learned that the user can reliably produce:

$$
\text{Gather} \rightarrow \text{Route} \rightarrow \text{Shape}
$$

without needing every verbal cue.

This progression could be based on **actual demonstrated competency**, not an XP unlock.

That's a huge distinction.

---

### Advanced — Silent casting

Eventually:

```text
core activation
↓
mana routing
↓
spell shaping intention
↓
release
```

No speech at all.

This is essentially what *Mushoku Tensei* describes: incantations provide a conventional activation mechanism, whereas silent casting involves directly manipulating and shaping mana. [S77]

The amazing part is that your version could make the player **actually learn the silent version**.

They aren't pressing an unlock button saying:

> Silent Casting acquired.

They've trained the corresponding bodily control until they no longer need the verbal scaffolding.

That's excellent game design *and* an interesting experiment.

---

## Voice quality could matter too — but carefully

Once basic recognition works, the system could evaluate more than words.

Not whether someone has a "good voice."

Instead:

- cadence;
- pauses;
- continuity;
- emphasis;
- timing;
- confidence;
- breath stability.

Imagine casting a large spell.

You have to maintain a stable mana draw while speaking for eight seconds.

If your voice breaks because you lose control:

> the spell destabilizes.

If you rush:

> mana formation becomes inefficient.

If you maintain controlled breathing and pacing:

> the spell continues building.

Now chanting becomes an actual skill.

But I would make **pronunciation fairly forgiving**. Otherwise accents, speech differences, microphone quality and ordinary recognition errors become gameplay disadvantages.

---

## We already have the necessary voice technology

For predetermined magical incantations, this is considerably easier than general conversational speech recognition.

You essentially know the valid vocabulary ahead of time.

A current system such as Picovoice Rhino performs **on-device speech-to-intent**, where you define a limited context and it maps speech directly into known intents and parameters. Its current SDK runs locally across Windows/Linux and received a new context-wildcard feature in September 2026. [S79]

Alternatively, `whisper.cpp` can provide local transcription. There are already Unreal integrations using it for asynchronous offline speech recognition and voice-activity detection. [S80]

For your project I'd probably separate it:

```text
Headset microphone
        ↓
Voice Activity Detection
        ↓
Local speech recognizer
        ↓
Phrase alignment
        ↓
Chant Interpreter
        ↓
Spell State Machine
```

So Unreal gets events like:

```text
CHANT_PHRASE {
    spell: FIRE_LANCE
    phase: SHAPING
    confidence: 0.94
    timing_error_ms: 83
}
```

rather than constantly trying to interpret raw microphone audio itself.

---

## The system becomes multimodal

Now we're getting to something much closer to what I think your eventual project should become.

Suppose you're casting:

> **Greater Fire Lance**

The computer observes:

$$
X=
[
E_\text{core},
E_\text{arm},
R_\text{respiration},
V_\text{voice},
G_\text{gaze},
H_\text{hand}
]
$$

where:

- $E_\text{core}$ = abdominal EMG;
- $E_\text{arm}$ = routing EMG;
- $R$ = breathing;
- $V$ = chant;
- $G$ = gaze;
- $H$ = hand posture/intention.

The player experiences:

```text
              intention
                  ↓
             MANA CORE
                  ●
                ╱ │ ╲
           breathing EMG focus
                  │
                  ▼
               routing
                  │
            torso → arm
                  │
             haptic flow
                  ▼
                HAND
                  ✦
                  │
             incantation
                  │
                  ▼
              SPELL FORM
                  │
              RELEASE
                  │
                  ▼
                 🔥
```

No single signal needs to be extraordinarily reliable.

Together they're extremely informative.

---

## Different schools could feel fundamentally different

You mentioned the different schools earlier, and voice makes this dramatically more interesting.

I wouldn't just give each school different particle effects.

Give each a different **somatic + vocal grammar**.

### Fire

Fast accumulation.

Strong core → arm flow.

Short, forceful incantations.

Abrupt release.

Feels:

> explosive / aggressive / hot conceptually.

---

### Water

Slow continuous mana.

Smooth breathing.

Longer flowing phrases.

Sustained arm control.

Feels:

> fluid / continuous / controlled.

---

### Earth

Deep core engagement.

Minimal arm routing.

Slow, deliberate speech.

Large bass-shaker feedback.

Feels:

> heavy / grounded / resistant.

---

### Wind

Very light activation.

Breath-dominant.

Quick incantation.

Airflow feedback around the body.

Feels:

> light / fast / expansive.

---

### Healing

Completely different.

Slow respiration.

Mana routes:

```text
core
↓
chest
↓
both arms
↓
palms
```

Maybe spoken phrases are steady rather than explosive.

The player learns:

> offensive casting and healing literally **feel different inside my body.**

That's what would make the system special.

---

## You could even support compound magic

Eventually, once someone has learned two mana pathways:

```text
WATER
+
WIND
=
ICE
```

Casting might require transitioning between two learned patterns.

For instance:

```text
water-flow pattern
↓
maintain mana
↓
wind-control chant
↓
compression
↓
ice formation
```

This could be genuinely difficult.

Which is good.

You want a player eventually saying:

> "I can cast basic fire silently now, but I still need the full chant for advanced ice."

That sounds like fantasy roleplay, but underneath it is actually:

> "I've developed high reliability on one learned sensorimotor pattern but haven't automated another."

---

## There's another really interesting consequence: spell customization

Once players understand the system, you can allow them to alter parameters **during the chant**.

Imagine you've gathered a fireball.

Then you control:

### Power

Core intensity.

$$
P\propto M
$$

### Size

Duration of accumulation.

$$
r\propto \int M(t)\,dt
$$

### Velocity

Release intensity.

### Direction

Gaze + arm orientation.

### Shape

Specific vocal/motor modifier.

Now:

> Fireball

isn't one predefined ability.

It is a family of states.

One player casts:

> tiny extremely fast projectile.

Another:

> giant slow explosive orb.

Another:

> continuous stream.

That gets closer to fictional magic systems where proficient users are manipulating the underlying phenomenon rather than selecting spells from a toolbar.

---

## Eventually the chant becomes optional—but never useless

This is an important design decision.

Even a master silent caster should sometimes **want** to chant.

I'd make chanting provide something like:

$$
\text{efficiency}
\uparrow
$$

$$
\text{stability}
\uparrow
$$

$$
\text{maximum controllable mana}
\uparrow
$$

because the voice gives the brain an additional structured timing mechanism.

So:

**silent casting**

> fast, flexible, harder.

**shortened chant**

> balanced.

**full chant**

> slow, extremely stable, allows huge spells.

That gives you the classic fantasy moment:

> Skilled mage encounters something terrifying.

Normally they cast silently.

This time they stop.

Begin a long incantation.

The mana core starts hammering.

Haptics intensify.

The entire room starts reacting.

And you immediately understand:

> **Oh. I'm casting something enormous.**

That would be *absurdly* effective in VR.

---

And it gives this project a much more interesting ultimate objective than merely **"control VR using EMG."**

You're effectively trying to create a **learnable synthetic physiology**:

> mana core → internal energy → pathways → casting patterns → incantations → spells.

None of those biological systems actually exist, but if the sensory feedback, motor associations, voice cues and progression are sufficiently consistent, the user's brain may learn to interact with them *as though they form a coherent additional bodily system*.

That is the part of this experiment I'd be most excited to investigate.
