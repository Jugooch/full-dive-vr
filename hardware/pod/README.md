# Sensory recliner → pod

Built in Phase 4 ("sensory recliner v1"), **not** a sealed capsule. From the research:

```text
                 HEADSET
                 ╱─────╲
              headphones
                  │
          ┌───────┴───────┐
          │   headrest    │
       ╭──┴───────────────┴──╮
       │   shoulder haptics   │
       │                      │
       │    reclining body    │
       │                      │
       │ arm supports         │
       │      ╲        ╱      │
       │       ╲ legs ╱       │
       ╰──────────────────────╯
```

Design rules (see [`docs/safety.md`](../../docs/safety.md)):
- **Support, not restraint.** The body should be comfortable staying relaxed while the virtual body moves.
- **Mechanical safety fails open.** Always an immediate physical way out; no latches that need power.
- Base: a used recliner / zero-gravity chair + foam + modular mounts for motors, bass shaker, fans.
- Bass shaker mounted **structurally** to the frame.
- Cable management that never wraps a limb.

Later revisions (`recliner-v2`, pressure actuators on shoulders/back, articulated arm/leg supports) get
their own notes here, each with a safety review in the research log before first use.
