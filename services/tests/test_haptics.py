from partialdive.contracts import HapticEvent
from partialdive.haptics import HapticBus, HapticCondition, RecordingTransport

ZONES = {"core": 0, "chest": 1, "right_shoulder": 2, "right_forearm": 3, "right_hand": 4, "left_forearm": 5}


def bus(cond=None):
    t = RecordingTransport()
    return HapticBus(t, ZONES, {"air_front": 0}, cond), t


def test_immediate_contact():
    b, t = bus()
    b.handle(HapticEvent(1, 10.0, "contact", "right_forearm", 0.6, 120), now=10.0)
    b.tick(10.0)
    assert t.lines == ["H 3 153 120"]


def test_delay_condition():
    b, t = bus(HapticCondition(delay_ms=150))
    b.handle(HapticEvent(1, 10.0, "contact", "right_forearm", 1.0), now=10.0)
    assert b.tick(10.149) == 0
    assert b.tick(10.150) == 1


def test_remap_condition():
    b, t = bus(HapticCondition(zone_remap={"right_forearm": "left_forearm"}))
    b.handle(HapticEvent(1, 0.0, "contact", "right_forearm", 1.0), now=0.0)
    b.tick(1.0)
    assert t.lines == ["H 5 255 120"]


def test_disabled_condition_drops():
    b, t = bus(HapticCondition.from_params({"haptics_enabled": False}))
    b.handle(HapticEvent(1, 0.0, "contact", "chest", 1.0), now=0.0)
    b.tick(1.0)
    assert t.lines == [] and b.dropped == [(1, "condition")]


def test_flow_sequence_and_reduced():
    path = ["core", "chest", "right_shoulder", "right_forearm", "right_hand"]
    b, t = bus()
    b.handle(HapticEvent(1, 0.0, "flow", "core", 0.5, 80, path=path, step_ms=100), now=0.0)
    assert b.tick(0.0) == 1 and b.tick(0.25) == 2 and b.tick(0.5) == 2
    assert [l.split()[1] for l in t.lines] == ["0", "1", "2", "3", "4"]

    b, t = bus(HapticCondition.from_params({"mana_haptics": "reduced"}))
    b.handle(HapticEvent(2, 0.0, "flow", "core", 0.5, 80, path=path, step_ms=100), now=0.0)
    b.tick(5.0)
    assert [l.split()[1] for l in t.lines] == ["0", "2", "4"]


def test_mana_none_is_visual_only():
    assert HapticCondition.from_params({"mana_haptics": "none"}).enabled is False


def test_airflow_and_stop():
    b, t = bus()
    b.handle(HapticEvent(1, 0.0, "airflow", "air_front", 0.5), now=0.0)
    b.handle(HapticEvent(2, 0.0, "contact", "chest", 0.5), now=0.0)
    b.handle(HapticEvent(3, 0.0, "stop_all", "all", 0.0), now=0.0)
    b.tick(1.0)
    assert t.lines == ["S"]


def test_modalities_gate_kinds():
    b, t = bus(HapticCondition.from_params({"modalities": ["airflow"]}))
    b.handle(HapticEvent(1, 0.0, "contact", "chest", 1.0), now=0.0)
    b.handle(HapticEvent(2, 0.0, "airflow", "air_front", 1.0), now=0.0)
    b.tick(1.0)
    assert t.lines == ["F 0 255"]


def test_actuator_subset_and_reduced_zones():
    path = ["core", "chest", "right_shoulder", "right_forearm", "right_hand"]
    b, t = bus(HapticCondition.from_params({"actuators": ["core", "right_forearm"]}))
    b.handle(HapticEvent(1, 0.0, "flow", "core", 1.0, 80, path=path, step_ms=100), now=0.0)
    b.tick(5.0)
    assert [l.split()[1] for l in t.lines] == ["0", "3"]

    b, t = bus(HapticCondition.from_params({"mana_haptics": "reduced", "reduced_zones": ["core"],
                                            "amplitude_scale": 0.5}))
    b.handle(HapticEvent(2, 0.0, "flow", "core", 1.0, 80, path=path, step_ms=100), now=0.0)
    b.tick(5.0)
    assert t.lines == ["H 0 128 80"]
