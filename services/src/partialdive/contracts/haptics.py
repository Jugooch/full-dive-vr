"""HapticEvent — mirror of schemas/haptic-event.schema.json (haptic-event/1)."""

from __future__ import annotations

import json
from dataclasses import asdict, dataclass, field, fields

SCHEMA = "haptic-event/1"
KINDS = {"contact", "impact", "pulse", "flow", "rumble", "airflow", "stop_all"}
HAPTIC_ZONES = (
    "core", "chest", "upper_back",
    "left_shoulder", "right_shoulder",
    "left_upper_arm", "right_upper_arm",
    "left_forearm", "right_forearm",
    "left_hand", "right_hand",
    "left_torso", "right_torso",
    "left_thigh", "right_thigh",
    "left_foot", "right_foot",
    "seat", "air_front", "air_left", "air_right", "air_overhead", "all",
)


@dataclass
class HapticEvent:
    id: int
    timestamp: float
    kind: str
    zone: str
    strength: float
    duration_ms: int = 120
    pattern: str | None = None
    path: list[str] = field(default_factory=list)
    step_ms: int | None = None
    source: str | None = None

    def validate(self) -> None:
        if self.kind not in KINDS:
            raise ValueError(f"unknown kind {self.kind!r}")
        for z in (self.zone, *self.path):
            if z not in HAPTIC_ZONES:
                raise ValueError(f"unknown zone {z!r}")
        if not 0.0 <= self.strength <= 1.0:
            raise ValueError("strength outside [0, 1]")
        if not 0 <= self.duration_ms <= 10_000:
            raise ValueError("duration_ms outside [0, 10000]")
        if self.kind == "flow" and (not self.path or not self.step_ms):
            raise ValueError("flow events need path and step_ms")

    def to_json(self) -> bytes:
        self.validate()
        d = {"schema": SCHEMA, **{k: v for k, v in asdict(self).items() if v not in (None, [])}}
        return json.dumps(d, separators=(",", ":")).encode()

    @classmethod
    def from_dict(cls, d: dict) -> "HapticEvent":
        if d.get("schema") != SCHEMA:
            raise ValueError(f"expected {SCHEMA}, got {d.get('schema')!r}")
        known = {f.name for f in fields(cls)}
        event = cls(**{k: v for k, v in d.items() if k in known})
        event.validate()
        return event

    @classmethod
    def from_json(cls, raw: bytes | str) -> "HapticEvent":
        return cls.from_dict(json.loads(raw))
