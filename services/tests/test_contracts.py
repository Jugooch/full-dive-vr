import json
from pathlib import Path

import pytest

from partialdive.contracts import HAPTIC_ZONES, HapticEvent, IntentFrame
from partialdive.contracts.intent import UNIT_CHANNELS

SCHEMAS = Path(__file__).resolve().parents[2] / "schemas"


def test_intent_roundtrip():
    f = IntentFrame(seq=3, timestamp=1.5, source="emg", confidence=0.9, grab_right=1.0, extra={"x": 0.2})
    back = IntentFrame.from_json(f.to_json())
    assert back == f


def test_intent_rejects_out_of_range():
    with pytest.raises(ValueError):
        IntentFrame(seq=0, timestamp=0, source="emg", walk_forward=1.5).to_json()


def test_intent_fields_match_schema():
    props = json.loads((SCHEMAS / "intent-frame.schema.json").read_text())["properties"]
    for name in UNIT_CHANNELS:
        assert name in props
    assert set(props) - {"schema"} == {f for f in IntentFrame.__dataclass_fields__}


def test_haptic_zones_match_schema():
    schema = json.loads((SCHEMAS / "haptic-event.schema.json").read_text())
    assert tuple(schema["properties"]["zone"]["enum"]) == HAPTIC_ZONES


def test_flow_requires_path():
    with pytest.raises(ValueError):
        HapticEvent(1, 0.0, "flow", "core", 0.5).to_json()
