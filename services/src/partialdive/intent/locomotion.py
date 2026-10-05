"""Motionless locomotion (experiment 005): alternating small leg contractions -> walking.

v = k * f_step, where f_step is the detected virtual step frequency. Only *alternating* onsets count as
steps (L,R,L,R); a repeated same-leg onset restarts the cadence, so a single tense leg can't run away.
"""

from __future__ import annotations

from collections import deque


class StepCadence:
    def __init__(self, k: float = 0.5, window_s: float = 2.0, max_speed_mps: float = 1.6,
                 stop_after_s: float = 1.2):
        self.k, self.window_s, self.max_speed = k, window_s, max_speed_mps
        self.stop_after_s = stop_after_s
        self._steps: deque[float] = deque()
        self._last_leg: str | None = None
        self._prev = {"left": False, "right": False}

    def update(self, left_active: bool, right_active: bool, t: float) -> float:
        """Feed detector states each tick; returns walk_forward in [0, 1] (speed / max_speed)."""
        for leg, active in (("left", left_active), ("right", right_active)):
            if active and not self._prev[leg]:  # rising edge = step intent onset
                if self._last_leg is None or self._last_leg != leg:
                    self._steps.append(t)
                else:
                    self._steps.clear()
                    self._steps.append(t)
                self._last_leg = leg
            self._prev[leg] = active

        while self._steps and t - self._steps[0] > self.window_s:
            self._steps.popleft()
        if not self._steps or t - self._steps[-1] > self.stop_after_s or len(self._steps) < 2:
            return 0.0
        span = self._steps[-1] - self._steps[0]
        f_step = (len(self._steps) - 1) / span if span > 0 else 0.0
        return min(self.k * f_step / self.max_speed, 1.0)
