# 105 — Mana control language

- **Status:** planned
- **Version / Phase:** V3 (magic becomes a control language)
- **Branch:** Synthetic Physiology
- **Depends on:** 104
- **Research source:** [07-version-roadmap](../../docs/research/07-version-roadmap.md) (V3), [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md) (magic schools / somatic grammar)

## Question

Can users learn **different internal control patterns** and reliably link them to different virtual phenomena, so they manipulate a virtual physiological system rather than select spells?

## Hypothesis

Three operations (GATHER, ROUTE, SHAPE) combined into compositions (e.g. GATHER + ROUTE RIGHT + PROJECTILE vs GATHER + ROUTE BOTH ARMS + EXPAND), across three schools that differ in flow dynamics, can be produced and recognized reliably.

## Hardware

- 101 setup + left-forearm EMG (bilateral routing); haptic route for both arms

## Software

- Spell composer: operation sequence → `ManaSpell {element, form, behavior, magnitude, target}`
- Schools (initial three only):
  - **Fire**: fast, high-intensity flow; short, sharp haptic profile
  - **Water**: slow, sustained flow; smooth, rhythmic haptics
  - **Force / neutral**: baseline
- Haptic signature per school (routing timing, amplitude envelope)

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Neutral projectile | school: neutral, composition: [GATHER, ROUTE_RIGHT, PROJECTILE] |
| B | Fire projectile | school: fire, composition: [GATHER, ROUTE_RIGHT, PROJECTILE] |
| C | Water barrier | school: water, composition: [GATHER, ROUTE_BOTH, EXPAND] |

## Procedure

1. Learn each pattern (unscored).
2. Cued casting: a target spell is shown, and you produce the pattern. 10 per condition per session, randomized.
3. Blinded recognition probe (procedural choice): passive haptic+visual playback of a school signature, then you name the school.

## Measures

- Pattern-production accuracy (decoded composition = target), confusion between schools, cast time
- School-recognition accuracy from haptics alone
- [casting-experience](../instruments/casting-experience.md), [mana-sensation](../instruments/mana-sensation.md)

## Success criteria / gate

- Production accuracy well above chance for all 3 compositions over 2 or more sessions. Opens 106.

## Safety

See [safety](../../docs/safety.md).

## Analysis plan

Confusion matrices for production and recognition, tracked over sessions.

## Notes / open questions

- Earth, wind, healing and compound magic (water + wind = ice) are deliberately deferred. Add them only after the three-school test works.
