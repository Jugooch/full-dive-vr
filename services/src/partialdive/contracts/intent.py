"""IntentFrame — mirror of schemas/intent-frame.schema.json (intent-frame/1)."""

from __future__ import annotations

import json
from dataclasses import asdict, dataclass, field, fields

SCHEMA = "intent-frame/1"
SOURCES = {"joystick", "keyboard", "emg", "eeg", "gaze", "fusion", "replay", "simulated"}

# Continuous 0..1 channels. Order is irrelevant; names must match the schema exactly.
UNIT_CHANNELS = (
    "walk_forward", "step_left", "step_right", "turn_left", "turn_right",
    "grab_left", "grab_right", "action_strength",
    "core_activation", "route_left", "route_right", "release", "stillness",
)


@dataclass
class IntentFrame:
    seq: int
    timestamp: float
    source: str
    confidence: float = 0.0

    walk_forward: float = 0.0
    step_left: float = 0.0
    step_right: float = 0.0
    turn_left: float = 0.0
    turn_right: float = 0.0
    grab_left: float = 0.0
    grab_right: float = 0.0
    action_strength: float = 0.0

    core_activation: float = 0.0
    route_left: float = 0.0
    route_right: float = 0.0
    release: float = 0.0
    respiration_phase: float | None = None
    stillness: float = 0.0

    gaze_target: str | None = None
    extra: dict[str, float] = field(default_factory=dict)

    def validate(self) -> None:
        if self.source not in SOURCES:
            raise ValueError(f"unknown source {self.source!r}")
        if self.seq < 0:
            raise ValueError("seq must be >= 0")
        for name in (*UNIT_CHANNELS, "confidence"):
            value = getattr(self, name)
            if not 0.0 <= value <= 1.0:
                raise ValueError(f"{name}={value} outside [0, 1]")
        if self.respiration_phase is not None and not -1.0 <= self.respiration_phase <= 1.0:
            raise ValueError("respiration_phase outside [-1, 1]")

    def to_dict(self) -> dict:
        d = {"schema": SCHEMA, **asdict(self)}
        # Keep datagrams small: omit unset optionals and zero channels except the required ones.
        return {
            k: v for k, v in d.items()
            if k in ("schema", "seq", "timestamp", "source", "confidence") or v not in (0.0, None, {})
        }

    def to_json(self) -> bytes:
        self.validate()
        return json.dumps(self.to_dict(), separators=(",", ":")).encode()

    @classmethod
    def from_dict(cls, d: dict) -> "IntentFrame":
        if d.get("schema") != SCHEMA:
            raise ValueError(f"expected {SCHEMA}, got {d.get('schema')!r}")
        known = {f.name for f in fields(cls)}
        unknown = set(d) - known - {"schema"}
        if unknown:
            raise ValueError(f"unknown IntentFrame fields: {sorted(unknown)}")
        frame = cls(**{k: v for k, v in d.items() if k in known})
        frame.validate()
        return frame

    @classmethod
    def from_json(cls, raw: bytes | str) -> "IntentFrame":
        return cls.from_dict(json.loads(raw))
