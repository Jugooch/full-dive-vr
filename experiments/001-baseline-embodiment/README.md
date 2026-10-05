# 001 — Baseline embodiment

- **Status:** planned
- **Version / Phase:** V0 · Phase 1A
- **Branch:** Embodied Control
- **Depends on:** —
- **Research source:** [04-research-program-guide](../../docs/research/04-research-program-guide.md) (Phase 1, Experiment 1A)

## Question

How strongly do I embody a full first-person virtual body while reclined with conventional controllers, and how much does that vary from day to day?

## Hypothesis

A full avatar (torso, arms, hands, legs, feet) with the camera at the avatar's eyes gives stable, measurable VEQ ownership and agency. Day-to-day variance sets the noise floor for every later comparison.

## Hardware

- VR headset (Oculus Rift S) + VR-capable PC (the research says use what you have; don't upgrade yet)
- Comfortable reclining chair
- Standard controllers

## Software

- Unreal 5 + OpenXR **embodiment lab** scene: a room with a mirror, table, cube, sphere, sword and target dummy
- Full-body avatar with hands and feet (no floating controller hands), first-person camera at the avatar's eye position
- `partialdive session` for metadata; VEQ entry

## Conditions

| ID | Label | Key parameters |
|---|---|---|
| A | Baseline full-body avatar, controllers | avatar: full_body, input: controllers, posture: reclined |

## Procedure

1. Recline. Calibrate the avatar's height and eye position.
2. Do the standard sequence for 5–10 minutes: look at hands → open/close hands → look in mirror → touch virtual table → pick up cube → move arms → look down at body.
3. Fill in the VEQ immediately, still reclined.
4. Record: session, condition, duration, frame rate, headset, avatar model, VEQ ownership/agency/change, notes.
5. Repeat on **several different days** (at least 3 suggested).

## Measures

- [VEQ](../instruments/veq.md) ownership, agency, change
- Frame rate (average and minimum) and session duration
- [Metrics](../../docs/metrics.md): immersion category

## Success criteria / gate

- At least 3 sessions on different days, with mean and SD per VEQ subscale.
- These values become the reference baseline for 002, 006 and 008.

## Safety

See [safety](../../docs/safety.md). Stop if you feel cybersick. Reclined posture only, with a clear exit path.

## Analysis plan

Mean and SD per subscale across days. Note any session with a frame-rate drop and consider excluding it.

## Notes / open questions

- Does the mirror noticeably change ownership? It could become a later sub-condition.
- Decide the avatar model now and keep it fixed. Changing it invalidates comparisons with this baseline.
