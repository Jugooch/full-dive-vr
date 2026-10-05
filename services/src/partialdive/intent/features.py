"""Classic time-domain EMG features (experiment 004). Start here before any deep learning:
small biosignal datasets frequently do not justify large networks."""

from __future__ import annotations

import numpy as np


def mean_absolute_value(x: np.ndarray) -> float:
    return float(np.mean(np.abs(x)))


def rms(x: np.ndarray) -> float:
    return float(np.sqrt(np.mean(np.square(x))))


def variance(x: np.ndarray) -> float:
    return float(np.var(x))


def waveform_length(x: np.ndarray) -> float:
    return float(np.sum(np.abs(np.diff(x))))


def window_features(window: np.ndarray) -> np.ndarray:
    """window: (n_samples, n_channels) -> flat feature vector [MAV, RMS, VAR, WL, PEAK] per channel."""
    window = np.atleast_2d(window)
    if window.shape[0] == 1:
        window = window.T
    feats = []
    for ch in window.T:
        feats.extend([mean_absolute_value(ch), rms(ch), variance(ch), waveform_length(ch), float(np.max(ch))])
    return np.asarray(feats)
