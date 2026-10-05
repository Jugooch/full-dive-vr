from __future__ import annotations

import math

import numpy as np


def classification_metrics(pred, truth) -> dict:
    """Binary accuracy / precision / recall / F1 (positive = intent active)."""
    pred, truth = np.asarray(pred, bool), np.asarray(truth, bool)
    tp = int(np.sum(pred & truth))
    fp = int(np.sum(pred & ~truth))
    fn = int(np.sum(~pred & truth))
    acc = float(np.mean(pred == truth)) if len(pred) else float("nan")
    precision = tp / (tp + fp) if tp + fp else 0.0
    recall = tp / (tp + fn) if tp + fn else 0.0
    f1 = 2 * precision * recall / (precision + recall) if precision + recall else 0.0
    return {"accuracy": acc, "precision": precision, "recall": recall, "f1": f1}


def _rising_edges(active: np.ndarray) -> np.ndarray:
    a = np.asarray(active, bool).astype(np.int8)
    return np.flatnonzero(np.diff(a, prepend=0) == 1)


def false_activations_per_min(active, t, rest_mask) -> float:
    """Detector onsets that happen while the participant is instructed to rest, per minute of rest.
    Target from the research: < 1 per minute."""
    active, t, rest = np.asarray(active, bool), np.asarray(t, float), np.asarray(rest_mask, bool)
    onsets = _rising_edges(active)
    false = int(np.sum(rest[onsets])) if len(onsets) else 0
    dt = np.diff(t, append=t[-1])
    rest_minutes = float(np.sum(dt[rest])) / 60.0
    return false / rest_minutes if rest_minutes > 0 else float("nan")


def onset_latencies(cue_times, detected_times, max_lag_s: float = 1.0) -> np.ndarray:
    """For each cue, latency to the first detection within max_lag_s (NaN = missed intent)."""
    det = np.sort(np.asarray(detected_times, float))
    out = []
    for c in cue_times:
        i = np.searchsorted(det, c)
        out.append(det[i] - c if i < len(det) and det[i] - c <= max_lag_s else np.nan)
    return np.asarray(out)


def wolpaw_itr(n_classes: int, accuracy: float, seconds_per_selection: float) -> float:
    """Information transfer rate in bits/min (Wolpaw et al.)."""
    n, p = n_classes, min(max(accuracy, 0.0), 1.0)
    if n < 2 or seconds_per_selection <= 0:
        raise ValueError("need n_classes >= 2 and positive selection time")
    if p <= 1.0 / n:
        return 0.0
    bits = math.log2(n) + p * math.log2(p)
    if p < 1.0:
        bits += (1 - p) * math.log2((1 - p) / (n - 1))
    return bits * 60.0 / seconds_per_selection


def effort_ratio(virtual_task_emg, natural_task_emg) -> float:
    """E (and R in experiment 007) = muscle activation during the virtual task / during the real task.
    Long-term objective: E -> 0 while agency stays high. Uses mean envelope above zero."""
    v = float(np.mean(np.asarray(virtual_task_emg, float)))
    n = float(np.mean(np.asarray(natural_task_emg, float)))
    if n <= 0:
        raise ValueError("natural-task activation must be > 0")
    return v / n


def above_chance_p(correct: int, trials: int, chance: float = 0.5) -> float:
    """One-sided exact binomial p-value for >= `correct` successes under chance (held-out trials only)."""
    return float(sum(math.comb(trials, k) * chance**k * (1 - chance) ** (trials - k)
                     for k in range(correct, trials + 1)))
