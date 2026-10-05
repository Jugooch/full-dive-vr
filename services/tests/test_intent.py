import numpy as np

from partialdive.intent import ActivationNormalizer, RestCalibration, StepCadence, ThresholdDetector
from partialdive.intent.decoder import Calibration, EmgIntentDecoder


def cal():
    return RestCalibration(mu_rest=0.05, sigma_rest=0.01, mu_max=0.45)


def test_threshold_formula():
    assert abs(cal().threshold(4.0) - 0.09) < 1e-12


def test_detector_dwell_and_hysteresis():
    det = ThresholdDetector(cal(), k=4.0, min_on_ms=30, min_off_ms=60)
    assert det.update(0.3, 0.000) is False      # above T but dwell not met
    assert det.update(0.3, 0.031) is True
    assert det.update(0.08, 0.040) is True      # between T_off and T_on: stays on
    assert det.update(0.05, 0.050) is True
    assert det.update(0.05, 0.111) is False


def test_detector_ignores_spike():
    det = ThresholdDetector(cal(), k=4.0, min_on_ms=30)
    det.update(0.5, 0.0)
    assert det.update(0.05, 0.01) is False
    assert det.update(0.05, 0.05) is False


def test_normalizer_range_and_deadband():
    n = ActivationNormalizer(cal(), alpha=1.0)
    assert n.update(0.06) == 0.0
    assert n.update(0.25) == 0.5
    assert n.update(2.0) == 1.0


def test_cadence_requires_alternation():
    c = StepCadence(k=0.5, max_speed_mps=1.0)
    t, speeds = 0.0, []
    for leg in ["L", "R", "L", "R", "L"]:
        speeds.append(c.update(leg == "L", leg == "R", t))
        speeds.append(c.update(False, False, t + 0.1))
        t += 0.5
    assert speeds[-1] > 0.9  # 2 steps/s * 0.5 = 1.0 m/s -> 1.0
    same = StepCadence()
    for i in range(5):
        same.update(True, False, i * 0.5)
        v = same.update(False, False, i * 0.5 + 0.1)
    assert v == 0.0


def test_cadence_stops():
    c = StepCadence()
    c.update(True, False, 0.0); c.update(False, False, 0.1)
    c.update(False, True, 0.5)
    assert c.update(False, False, 0.6) > 0
    assert c.update(False, False, 3.0) == 0.0


def test_decoder_end_to_end():
    calib = Calibration({"right_forearm": cal(), "abdomen": cal()})
    dec = EmgIntentDecoder(
        ["right_forearm", "abdomen"],
        {"grab_right": {"mode": "binary", "channel": "right_forearm"},
         "core_activation": {"mode": "continuous", "channel": "abdomen", "alpha": 1.0}},
        calib,
    )
    frames = [dec.decode(i / 1000, np.array([0.4, 0.25])) for i in range(50)]
    assert frames[-1].grab_right == 1.0
    assert frames[-1].core_activation == 0.5
    assert frames[-1].seq == 49
    frames[-1].to_json()
