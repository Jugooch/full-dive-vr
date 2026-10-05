"""Profile-driven EMG intent decoder.

The hardware profile (hardware/profiles/*.yaml) says which EMG channel drives which IntentFrame field
and how. Swapping placements or adding a sensor is a YAML change, not a code change:

    intent_mapping:
      grab_right:      {mode: binary, channel: right_forearm, k: 4.0}
      core_activation: {mode: continuous, channel: abdomen}
      walk_forward:    {mode: cadence, left: left_quad, right: right_quad, k: 4.0, speed_k: 0.5}
"""

from __future__ import annotations

from dataclasses import dataclass, field

import numpy as np

from ..contracts import IntentFrame
from .locomotion import StepCadence
from .threshold import ActivationNormalizer, RestCalibration, ThresholdDetector


@dataclass
class Calibration:
    """Per-channel rest/max calibration, saved alongside the session for reproducibility."""

    channels: dict[str, RestCalibration] = field(default_factory=dict)

    def to_dict(self) -> dict:
        return {name: vars(c) for name, c in self.channels.items()}

    @classmethod
    def from_dict(cls, d: dict) -> "Calibration":
        return cls({name: RestCalibration(**v) for name, v in d.items()})


class EmgIntentDecoder:
    def __init__(self, channel_names: list[str], mapping: dict[str, dict], cal: Calibration):
        self.index = {name: i for i, name in enumerate(channel_names)}
        self.seq = 0
        self._binary: dict[str, tuple[int, ThresholdDetector]] = {}
        self._continuous: dict[str, tuple[int, ActivationNormalizer]] = {}
        self._cadence: dict[str, tuple[int, int, ThresholdDetector, ThresholdDetector, StepCadence]] = {}

        for field_name, spec in mapping.items():
            mode = spec["mode"]
            if mode == "binary":
                ch = spec["channel"]
                det = ThresholdDetector(cal.channels[ch], k=spec.get("k", 4.0))
                self._binary[field_name] = (self.index[ch], det)
            elif mode == "continuous":
                ch = spec["channel"]
                norm = ActivationNormalizer(cal.channels[ch], alpha=spec.get("alpha", 0.2))
                self._continuous[field_name] = (self.index[ch], norm)
            elif mode == "cadence":
                l, r = spec["left"], spec["right"]
                k = spec.get("k", 4.0)
                self._cadence[field_name] = (
                    self.index[l], self.index[r],
                    ThresholdDetector(cal.channels[l], k=k),
                    ThresholdDetector(cal.channels[r], k=k),
                    StepCadence(k=spec.get("speed_k", 0.5)),
                )
            else:
                raise ValueError(f"unknown mode {mode!r} for {field_name}")

    def decode(self, t: float, values: np.ndarray) -> IntentFrame:
        frame = IntentFrame(seq=self.seq, timestamp=t, source="emg", confidence=1.0)
        self.seq += 1
        for name, (i, det) in self._binary.items():
            setattr(frame, name, 1.0 if det.update(float(values[i]), t) else 0.0)
        for name, (i, norm) in self._continuous.items():
            setattr(frame, name, norm.update(float(values[i])))
        for name, (li, ri, ldet, rdet, cadence) in self._cadence.items():
            left = ldet.update(float(values[li]), t)
            right = rdet.update(float(values[ri]), t)
            frame.step_left, frame.step_right = float(left), float(right)
            setattr(frame, name, cadence.update(left, right, t))
        return frame
