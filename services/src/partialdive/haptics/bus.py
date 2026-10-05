"""The haptic bus is where experiment conditions live.

Unreal always emits the *true* virtual event. The bus decides what the body actually receives:
delayed (experiment 002: +150 ms, >300 ms), spatially remapped (right arm -> left arm), scaled, reduced,
or suppressed entirely (visual-only catch trials in 104). Keeping this out of Unreal means the game can't
accidentally leak the condition, and every manipulation is logged in one place.
"""

from __future__ import annotations

import heapq
import itertools
from dataclasses import dataclass, field

from ..contracts import HapticEvent

MAX_STRENGTH = 255


@dataclass
class HapticCondition:
    enabled: bool = True
    delay_ms: float = 0.0
    zone_remap: dict[str, str] = field(default_factory=dict)
    strength_scale: float = 1.0
    allowed_kinds: set[str] | None = None  # None = all kinds
    allowed_zones: set[str] | None = None  # None = every wired zone (102 actuator subsets, 104 reduced)
    flow_keep_every: int = 1  # "reduced haptics" without explicit zones: keep every Nth zone of a flow

    # Experiment 008 modality names -> event kinds the bus will pass through.
    MODALITY_KINDS = {
        "vibration": {"contact", "impact", "pulse", "flow"},
        "airflow": {"airflow"},
        "bass_shaker": {"rumble"},
    }

    @classmethod
    def from_params(cls, params: dict | None) -> "HapticCondition":
        """Build from an experiments/*/conditions.yaml `params` block (unknown keys ignored)."""
        p = params or {}
        mana = p.get("mana_haptics")  # full | reduced | none
        kinds = set(p["haptic_kinds"]) if p.get("haptic_kinds") else None
        if "modalities" in p:
            kinds = set().union(*(cls.MODALITY_KINDS[m] for m in p["modalities"])) | {"stop_all"}
        zones = p.get("actuators")
        if mana == "reduced" and p.get("reduced_zones"):
            zones = p["reduced_zones"]
        return cls(
            enabled=p.get("haptics_enabled", True) and mana != "none",
            delay_ms=float(p.get("haptic_delay_ms", 0) or 0),
            zone_remap=dict(p.get("haptic_zone_remap") or {}),
            strength_scale=float(p.get("haptic_strength_scale", p.get("amplitude_scale", 1.0))),
            allowed_kinds=kinds,
            allowed_zones=set(zones) if zones is not None else None,
            flow_keep_every=2 if mana == "reduced" and zones is None else int(p.get("flow_keep_every", 1)),
        )


@dataclass(order=True)
class ScheduledCommand:
    due: float
    order: int
    line: str = field(compare=False)
    event_id: int = field(compare=False)


class HapticBus:
    def __init__(self, transport, zone_channels: dict[str, int], fan_channels: dict[str, int] | None = None,
                 condition: HapticCondition | None = None, on_dispatch=None):
        self.transport = transport
        self.zone_channels = zone_channels
        self.fan_channels = fan_channels or {}
        self.condition = condition or HapticCondition()
        self.on_dispatch = on_dispatch  # callback(ScheduledCommand, now) for LSL logging / latency
        self._queue: list[ScheduledCommand] = []
        self._order = itertools.count()
        self.dropped: list[tuple[int, str]] = []

    def set_condition(self, condition: HapticCondition) -> None:
        self.condition = condition

    def handle(self, event: HapticEvent, now: float) -> None:
        cond = self.condition
        if event.kind == "stop_all":
            self._queue.clear()
            self.transport.stop_all()
            return
        if not cond.enabled or (cond.allowed_kinds is not None and event.kind not in cond.allowed_kinds):
            self.dropped.append((event.id, "condition"))
            return

        start = max(event.timestamp, now) + cond.delay_ms / 1000.0
        strength = min(event.strength * cond.strength_scale, 1.0)

        if event.kind == "flow":
            path = event.path[:: cond.flow_keep_every] if cond.flow_keep_every > 1 else event.path
            if cond.flow_keep_every > 1 and event.path[-1] not in path:
                path = [*path, event.path[-1]]  # always keep the destination
            for i, zone in enumerate(path):
                self._schedule_zone(event, zone, strength, event.duration_ms, start + i * event.step_ms / 1000.0)
        elif event.kind == "airflow":
            fan = self.fan_channels.get(event.zone)
            if fan is None:
                self.dropped.append((event.id, f"no fan for {event.zone}"))
                return
            self._push(start, f"F {fan} {round(strength * MAX_STRENGTH)}", event.id)
        else:
            self._schedule_zone(event, event.zone, strength, event.duration_ms, start)

    def _schedule_zone(self, event: HapticEvent, zone: str, strength: float, duration_ms: int, due: float):
        zone = self.condition.zone_remap.get(zone, zone)
        if self.condition.allowed_zones is not None and zone not in self.condition.allowed_zones:
            self.dropped.append((event.id, f"zone {zone} disabled by condition"))
            return
        ch = self.zone_channels.get(zone)
        if ch is None:
            self.dropped.append((event.id, f"no actuator for {zone}"))
            return
        self._push(due, f"H {ch} {round(strength * MAX_STRENGTH)} {int(duration_ms)}", event.id)

    def _push(self, due: float, line: str, event_id: int) -> None:
        heapq.heappush(self._queue, ScheduledCommand(due, next(self._order), line, event_id))

    def tick(self, now: float) -> int:
        """Send every command that is due. Call at >= 1 kHz for ms-accurate delays."""
        sent = 0
        while self._queue and self._queue[0].due <= now:
            cmd = heapq.heappop(self._queue)
            self.transport.send(cmd.line)
            if self.on_dispatch:
                self.on_dispatch(cmd, now)
            sent += 1
        return sent

    def pending(self) -> int:
        return len(self._queue)
