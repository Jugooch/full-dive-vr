"""Lab Streaming Layer helpers.

LSL is adopted from day one (see docs/decisions/0004-lsl-xdf-recording.md): every stream shares one clock
and LabRecorder writes them all to a single XDF per session. pylsl is optional so the stack still runs
on a machine without it; in that case timestamps fall back to a monotonic clock and nothing is streamed.
"""

from __future__ import annotations

import json
import time

try:  # pragma: no cover - depends on optional install
    import pylsl
except ImportError:  # pragma: no cover
    pylsl = None

HAVE_LSL = pylsl is not None


def local_clock() -> float:
    """Seconds on the LSL clock (or a monotonic fallback). Use this for every timestamp."""
    return pylsl.local_clock() if pylsl else time.perf_counter()


class MarkerOutlet:
    """Irregular-rate string stream for events (haptic commands, conditions, chant phrases)."""

    def __init__(self, name: str, source_id: str | None = None):
        self._outlet = None
        if pylsl:
            info = pylsl.StreamInfo(name, "Markers", 1, pylsl.IRREGULAR_RATE, "string", source_id or name)
            self._outlet = pylsl.StreamOutlet(info)

    def push(self, payload: dict | str, timestamp: float | None = None) -> None:
        if self._outlet is None:
            return
        text = payload if isinstance(payload, str) else json.dumps(payload, separators=(",", ":"))
        self._outlet.push_sample([text], timestamp or local_clock())


class SignalOutlet:
    """Regular-rate float stream for raw/processed biosignals."""

    def __init__(self, name: str, kind: str, channel_names: list[str], rate_hz: float):
        self._outlet = None
        if pylsl:
            info = pylsl.StreamInfo(name, kind, len(channel_names), rate_hz, "float32", name)
            chans = info.desc().append_child("channels")
            for ch in channel_names:
                chans.append_child("channel").append_child_value("label", ch)
            self._outlet = pylsl.StreamOutlet(info)

    def push(self, values, timestamp: float | None = None) -> None:
        if self._outlet is not None:
            self._outlet.push_sample(list(map(float, values)), timestamp or local_clock())
