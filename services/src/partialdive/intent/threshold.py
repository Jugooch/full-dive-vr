"""Threshold detection and continuous activation (experiments 003, 007, 101).

Research rule: determine whether the signal is separable before reaching for ML.
    T = mu_rest + k * sigma_rest ;  x > T  =>  intent active
Continuous activation (mana core, action strength):
    C = (x - mu_rest) / (mu_max - mu_rest),  clipped to [0, 1]
"""

from __future__ import annotations

from dataclasses import dataclass

import numpy as np


@dataclass
class RestCalibration:
    mu_rest: float
    sigma_rest: float
    mu_max: float | None = None

    @classmethod
    def fit(cls, rest: np.ndarray, maximal: np.ndarray | None = None) -> "RestCalibration":
        """rest: envelope samples recorded while relaxed. maximal: samples from a comfortable
        (not all-out) deliberate contraction, used as the top of the 0..1 range."""
        rest = np.asarray(rest, dtype=float)
        mu_max = float(np.percentile(maximal, 95)) if maximal is not None and len(maximal) else None
        return cls(float(rest.mean()), float(rest.std()), mu_max)

    def threshold(self, k: float) -> float:
        return self.mu_rest + k * self.sigma_rest


class ThresholdDetector:
    """Binary intent with hysteresis and minimum dwell times to suppress chatter and false activations.

    on when x > T_on for >= min_on_ms; off when x < T_off for >= min_off_ms, where
    T_on = mu + k*sigma and T_off = mu + k_off*sigma (k_off < k).
    """

    def __init__(self, cal: RestCalibration, k: float = 4.0, k_off: float | None = None,
                 min_on_ms: float = 30.0, min_off_ms: float = 60.0):
        if k_off is None:
            k_off = k * 0.6
        if k_off > k:
            raise ValueError("k_off must be <= k")
        self.t_on = cal.threshold(k)
        self.t_off = cal.threshold(k_off)
        self.min_on_s, self.min_off_s = min_on_ms / 1000, min_off_ms / 1000
        self.active = False
        self._pending_since: float | None = None

    def update(self, x: float, t: float) -> bool:
        crossing = (x > self.t_on) if not self.active else (x < self.t_off)
        if not crossing:
            self._pending_since = None
            return self.active
        if self._pending_since is None:
            self._pending_since = t
        dwell = self.min_on_s if not self.active else self.min_off_s
        if t - self._pending_since >= dwell:
            self.active = not self.active
            self._pending_since = None
        return self.active


class ActivationNormalizer:
    """C in [0,1] with optional exponential smoothing (alpha=1 disables smoothing)."""

    def __init__(self, cal: RestCalibration, alpha: float = 0.2, deadband_k: float = 2.0):
        if cal.mu_max is None or cal.mu_max <= cal.mu_rest:
            raise ValueError("calibration needs mu_max > mu_rest; record a deliberate contraction")
        self.cal, self.alpha = cal, alpha
        self.floor = cal.threshold(deadband_k)  # below this, report exactly 0 (rest is rest)
        self.value = 0.0

    def update(self, x: float) -> float:
        raw = 0.0 if x <= self.floor else (x - self.cal.mu_rest) / (self.cal.mu_max - self.cal.mu_rest)
        raw = min(max(raw, 0.0), 1.0)
        self.value += self.alpha * (raw - self.value)
        return self.value
