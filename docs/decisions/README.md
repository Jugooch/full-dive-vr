# Decision records

Short records of decisions that shape the project, so future-you knows *why*. Copy
[`0000-template.md`](0000-template.md), number sequentially, never delete; supersede instead.

| # | Decision | Status |
|---|---|---|
| [0001](0001-emg-before-eeg.md) | Prove interaction with EMG before buying EEG | accepted |
| [0002](0002-unreal-openxr.md) | Unreal Engine 5 + OpenXR, PCVR/Link | accepted |
| [0003](0003-intent-bus.md) | Intent Bus: gameplay consumes only `IntentFrame`s | accepted |
| [0004](0004-lsl-xdf-recording.md) | LSL clock + XDF recording from day one | accepted |
| [0005](0005-read-only-safety-boundary.md) | Read-only sensing + physical feedback only | accepted |
| [0006](0006-conditions-in-haptic-bus.md) | Experiment conditions applied in the haptic bus, blinded | accepted |
| [0007](0007-two-branch-roadmap.md) | Two branches (embodied control, synthetic physiology) on one platform | accepted |

Decisions still to make (write an ADR when the time comes): voice engine for V4 (Rhino vs whisper.cpp);
EEG board purchase (Ganglion first?); proficiency-score weights before experiment 107; partial-dive
composite score weights; any thermal or pressure actuator.
