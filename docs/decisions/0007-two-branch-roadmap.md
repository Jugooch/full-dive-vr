# 0007 — Two branches on one platform

- Status: accepted
- Date: 2026-10-05

## Context
The original plan was "V0 = walking, V2 = magic". The research revised it: synthetic physiology shouldn't
wait for locomotion to be finished. The branches test different hypotheses on the same platform, and the
mana system may be easier to make compelling than motionless walking ([research/07](../research/07-version-roadmap.md)).

## Decision
Versions V0–V6 as in [`roadmap.md`](../roadmap.md). Experiment IDs: `0xx` embodied control (+ EEG track
010–013), `1xx` synthetic physiology, `2xx` integration. The two branches merge only at V6.

## Consequences
Hardware and software are bought and built once and reused (EMG, haptics, ESP32s, Unreal bridge,
logging). Work on V2 can start around the time V0/V1 is working.
