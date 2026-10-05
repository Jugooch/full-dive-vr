"""Intent decoding: biosignal samples -> IntentFrame. Raw signals never leave this package."""

from .features import mean_absolute_value, rms, variance, waveform_length, window_features
from .locomotion import StepCadence
from .threshold import ActivationNormalizer, RestCalibration, ThresholdDetector

__all__ = [
    "ActivationNormalizer",
    "RestCalibration",
    "StepCadence",
    "ThresholdDetector",
    "mean_absolute_value",
    "rms",
    "variance",
    "waveform_length",
    "window_features",
]
