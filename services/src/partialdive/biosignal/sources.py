"""EMG sources.

`SerialEMGSource` reads the ESP32 `emg-streamer` firmware (see firmware/emg-streamer/README.md):
    E,<device_millis>,<env_mv0>,<env_mv1>,...\n
Values are the MyoWare ENV (envelope) voltage at the sensor in millivolts (the firmware undoes the
input divider), normalized here by `full_scale_mv` (the Power Shield's 4.2 V maximum). The research
recommends starting with the envelope rather than raw EMG.

`SimulatedEMGSource` produces plausible rest noise + contraction bursts so every downstream piece
(decoder, Unreal bridge, haptic loop) can be developed before hardware arrives.
"""

from __future__ import annotations

import math
import random
from collections.abc import Callable, Iterator
from dataclasses import dataclass

import numpy as np

from ..lsl import local_clock


@dataclass
class Sample:
    timestamp: float  # LSL clock seconds (host receive time)
    values: np.ndarray  # one value per channel, normalized to 0..1 of ADC full scale


class SerialEMGSource:
    def __init__(self, port: str, n_channels: int, baud: int = 921600, full_scale_mv: float = 4200.0):
        import serial  # optional dependency: pip install partialdive[hardware]

        self.n_channels = n_channels
        self.full_scale_mv = full_scale_mv
        self._ser = serial.Serial(port, baud, timeout=1)

    def __iter__(self) -> Iterator[Sample]:
        while True:
            line = self._ser.readline()
            if not line:
                continue
            parts = line.decode(errors="replace").strip().split(",")
            if len(parts) != self.n_channels + 2 or parts[0] != "E":
                continue  # banner, partial line, or status message
            try:
                raw = np.array([float(p) for p in parts[2:]])
            except ValueError:
                continue
            yield Sample(local_clock(), raw / self.full_scale_mv)

    def close(self) -> None:
        self._ser.close()


class SimulatedEMGSource:
    """Rest noise plus envelope bursts while `active(channel, t)` returns True."""

    def __init__(
        self,
        n_channels: int,
        rate_hz: float = 500.0,
        active: Callable[[int, float], bool] | None = None,
        rest_level: float = 0.05,
        flex_level: float = 0.45,
        noise: float = 0.01,
        seed: int | None = None,
        realtime: bool = False,
    ):
        self.n_channels = n_channels
        self.rate_hz = rate_hz
        # Default: adjacent channels alternate at 1 Hz, so a left/right leg pair produces walking cadence.
        self.active = active or (lambda ch, t: math.sin(2 * math.pi * 1.0 * t + ch * math.pi) > 0.6)
        self.rest_level, self.flex_level, self.noise = rest_level, flex_level, noise
        self._rng = random.Random(seed)
        self.realtime = realtime

    def __iter__(self) -> Iterator[Sample]:
        import time

        t0 = local_clock()
        n = 0
        while True:
            t = n / self.rate_hz
            vals = np.array([
                (self.flex_level if self.active(ch, t) else self.rest_level)
                + self._rng.gauss(0, self.noise)
                for ch in range(self.n_channels)
            ])
            yield Sample(t0 + t, np.clip(vals, 0.0, 1.0))
            n += 1
            if self.realtime:
                delay = t0 + n / self.rate_hz - local_clock()
                if delay > 0:
                    time.sleep(delay)
