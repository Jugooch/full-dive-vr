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

## 2026-10-05 — Repository set up
- Research conversation archived and split into the research record ([`research/`](research/)), with all 80 sources resolved.
- Repository scaffolded per the research's Phase 0: contracts (`schemas/`), Python services (`partialdive`:
  EMG acquisition + simulator, threshold/continuous/cadence intent decoders, haptic bus with condition
  manipulation, blinded session tooling, metrics), ESP32 firmware (EMG streamer, haptics controller),
  Unreal project spec, 22 experiment protocols (000–201), instruments, roadmap, safety rules.
- Next: create the Unreal project from the VR template (see `unreal/README.md`); order V0 hardware
  (`hardware/bom.csv`, stages 1–2); run `partialdive decode --simulate` against the Unreal bridge to
  test the loop before the hardware arrives.
