# 0002 — Unreal Engine 5 + OpenXR, PCVR/Link

- Status: accepted
- Date: 2026-10-05

## Context
UE 5.8 ships a VR template and OpenXR support; Epic and Meta recommend native OpenXR for new XR work, and
Meta Link lets you run Unreal scenes directly on a Quest while developing [S45–S48]
([research/04 Phase 0](../research/04-research-program-guide.md)).

## Decision
Unreal Engine 5 (5.8 at time of writing) with OpenXR, PCVR via Link. Biosignals stay *outside* the
engine in Python services connected by the contracts in `/schemas`.

## Consequences
Headsets can be swapped without touching the experimental core. C++ plugin work is needed for the UDP/LSL
bridge. Binary assets require Git LFS.
