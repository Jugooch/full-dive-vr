# 0004 — LSL clock + XDF recording from day one

- Status: accepted
- Date: 2026-10-05

## Context
LSL exists to synchronize streams (EEG, eye tracking, events) with sub-millisecond timing on a local
network and records to XDF via LabRecorder [S49, S50]. The research says to adopt it immediately, even
before owning an EEG ([research/04 Phase 0](../research/04-research-program-guide.md)).

## Decision
All timestamps use the LSL clock. Every service publishes `pdive.*` streams; each session is one XDF.
Raw XDF stays out of git history (`data/README.md`).

## Consequences
Latency and synchrony experiments (000, 002) are measurable. EEG later drops into the same recording.
`pylsl` is optional so development works without it.
