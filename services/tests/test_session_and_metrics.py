import math

import numpy as np

from partialdive.analysis import (
    above_chance_p, classification_metrics, effort_ratio, false_activations_per_min, onset_latencies,
    wolpaw_itr,
)
from partialdive.session import blinded_order

CONDS = [{"id": c, "params": {"haptic_delay_ms": d}} for c, d in zip("ABCDE", [0, 150, 350, 0, 0])]


def test_blinded_order_is_balanced_and_reproducible():
    a = blinded_order(CONDS, 3, seed=42)
    b = blinded_order(CONDS, 3, seed=42)
    assert [x.condition_id for x in a] == [x.condition_id for x in b]
    assert len({x.code for x in a}) == 15
    for r in range(3):
        assert sorted(x.condition_id for x in a[r * 5:(r + 1) * 5]) == list("ABCDE")


def test_metrics():
    m = classification_metrics([1, 1, 0, 0], [1, 0, 0, 0])
    assert m["accuracy"] == 0.75 and m["precision"] == 0.5 and m["recall"] == 1.0
    t = np.arange(0, 120, 0.5)
    active = np.zeros_like(t, bool); active[[10, 11, 50]] = True
    assert abs(false_activations_per_min(active, t, np.ones_like(t, bool)) - 2 / (119.5 / 60)) < 1e-9
    lat = onset_latencies([1.0, 5.0], [1.12, 9.0])
    assert abs(lat[0] - 0.12) < 1e-9 and math.isnan(lat[1])
    assert wolpaw_itr(2, 1.0, 4.0) == 15.0
    assert wolpaw_itr(2, 0.5, 4.0) == 0.0
    assert effort_ratio([0.1], [0.4]) == 0.25
    assert above_chance_p(10, 10) == 0.5 ** 10


def test_randomize_modes():
    fixed = blinded_order(CONDS, 2, seed=1, randomize=False)
    assert [x.condition_id for x in fixed] == list("ABCDEABCDE")
    full = blinded_order(CONDS, 4, seed=1, randomize="full")
    ids = [x.condition_id for x in full]
    assert sorted(ids) == sorted(list("ABCDE") * 4)
