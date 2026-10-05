# Research log

Dated lab notebook. Newest entries at the top. One entry per session, plus entries for build milestones,
decisions, surprises and failures. Link the session directory and the experiment.

Template:

```markdown
## YYYY-MM-DD — <experiment id> session sNN  (or: build / decision / note)
- Session: data/sessions/<experiment>/<YYYY-MM-DD>_sNN/
- Hardware profile / build: ...
- What ran: ...
- Result (after unblinding): ...
- Surprises / problems: ...
- Next: ...
```

---

## 2026-10-05 — Unreal project + PartialDiveBridge
- Unreal 5.8 VR template project created (`unreal/PartialDiveVR/PartialDiveVR/`), converted to C++.
- Headset: Oculus Rift S (PCVR, inside-out tracking, OpenXR via the Meta PC app). Quest not needed.
- LSL plugin added as a submodule (`labstreaminglayer/plugin-UE4`); builds on 5.8 though only listed up to 5.7.
- `PartialDiveBridge` v0.1 implemented: IntentFrame/block/chant UDP in, HapticEvent UDP out, LSL `pdive.game`
  markers, 250 ms stale-intent stop, input-source switch. `PartialDive.Json.*` automation tests pass.
- Problem: build failed with `VisualStudioTools` not found in `UE5Rules`. The engine's precompiled rules cache
  predated the VS integration plugin. Deleting the cache wasn't enough on an installed engine; it was
  regenerated once with `InstalledBuild.txt` temporarily renamed (see `unreal/README.md` troubleshooting).
- Smoke test passed: simulated intent visible in PIE (walk 0.625). Fixed a one-frame input lag (bridge now drains
  UDP at world tick start); added `pdive.debug` overlay and `partialdive dev` (decoder + haptic bus in one command).
- Next: full-body avatar driven by `GetIntent()`; experiment 000 loop-latency scene.

## 2026-10-05 — Repository set up
- Research conversation archived and split into the research record ([`research/`](research/)), with all 80 sources resolved.
- Repository scaffolded per the research's Phase 0: contracts (`schemas/`), Python services (`partialdive`:
  EMG acquisition + simulator, threshold/continuous/cadence intent decoders, haptic bus with condition
  manipulation, blinded session tooling, metrics), ESP32 firmware (EMG streamer, haptics controller),
  Unreal project spec, 22 experiment protocols (000–201), instruments, roadmap, safety rules.
- Next: create the Unreal project from the VR template (see `unreal/README.md`); order V0 hardware
  (`hardware/bom.csv`, stages 1–2); run `partialdive decode --simulate` against the Unreal bridge to
  test the loop before the hardware arrives.
