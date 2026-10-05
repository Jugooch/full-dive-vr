# 106 — Incantations

- **Status:** planned
- **Version / Phase:** V4
- **Branch:** Synthetic Physiology
- **Depends on:** 105
- **Research source:** [06-incantations-and-casting](../../docs/research/06-incantations-and-casting.md), [07-version-roadmap](../../docs/research/07-version-roadmap.md) (V4)

## Question

Does structured speech make synthetic physiology easier to learn and control? Chants are added only after 105 shows that synthetic physiology works on its own, so that speech isn't a confounding variable.

## Hypothesis

A structured chant whose phrases match the casting stages (C) gives better stability, accuracy and learning speed than no speech (A) or an arbitrary phrase (B).

## Hardware

- 105 setup + headset microphone

## Software

- Voice pipeline: headset mic → voice activity detection → local recognizer (Picovoice Rhino speech-to-intent, or whisper.cpp) → phrase alignment → chant interpreter → spell state machine
- Unreal receives `CHANT_PHRASE {spell, phase, confidence, timing_error_ms}` events, never raw audio
- Phrase → stage mapping (example): phrase 1 → gather, phrase 2 → route, phrase 3 → shape, phrase 4 → release
- Cast quality Q = w_m·M + w_v·V + w_c·C + w_f·F, **felt rather than shown** (unstable vibration for bad routing, distorted formation for a bad chant)
- **Forgiving pronunciation**: accents, speech differences and microphone quality must not count against the caster

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | No speech | chant: none |
| B | Arbitrary phrase | chant: arbitrary |
| C | Structured chant | chant: structured |

## Procedure

1. Learn the phrases for B and C. Use original fictional incantations.
2. 10 cued casts per condition per session, randomized. 3 or more sessions.
3. Casting-experience after each block.

## Measures

- Cast success, Q components (M, V, C, F), stability, cast time; recognizer confidence and timing error
- [casting-experience](../instruments/casting-experience.md) incl. CE9; [mana-sensation](../instruments/mana-sensation.md)

## Success criteria / gate

- C beats A and B on stability or learning rate. Opens 107.

## Safety

See [safety](../../docs/safety.md). Long chants must not need breath-holding. Pace them for comfortable breathing.

## Analysis plan

Per-condition success and stability over sessions. Recognizer error analysis, kept separate from player error.

## Notes / open questions

- Later: voice qualities (cadence, pauses, continuity, breath stability) feeding stability. Keep this forgiving.
