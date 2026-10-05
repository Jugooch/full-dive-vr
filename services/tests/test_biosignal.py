import numpy as np

from partialdive.biosignal import SerialEMGSource


class FakeSerial:
    def __init__(self, lines):
        self.lines = [l.encode() for l in lines]

    def readline(self):
        return self.lines.pop(0) if self.lines else b""


def test_serial_source_parses_millivolts_and_skips_status_lines():
    src = SerialEMGSource.__new__(SerialEMGSource)  # skip opening a real port
    src.n_channels, src.full_scale_mv = 2, 4200.0
    src._ser = FakeSerial([
        "# emg-streamer 0.2.0 channels=2 rate=1000 units=mV divider=1.800\n",
        "E,10,210\n",            # wrong channel count -> skipped
        "E,11,2100,4200\n",
        "garbage\n",
        "E,12,0,420\n",
    ])
    it = iter(src)
    a, b = next(it), next(it)
    assert np.allclose(a.values, [0.5, 1.0])
    assert np.allclose(b.values, [0.0, 0.1])
