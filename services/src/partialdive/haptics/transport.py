"""Transports speak the haptic-command/1 serial line protocol (schemas/haptic-command.md)."""

from __future__ import annotations


class SerialHapticTransport:
    def __init__(self, port: str, baud: int = 115200):
        import serial  # optional dependency: pip install partialdive[hardware]

        self._ser = serial.Serial(port, baud, timeout=0.05)

    def send(self, line: str) -> None:
        self._ser.write((line + "\n").encode())

    def stop_all(self) -> None:
        self.send("S")

    def close(self) -> None:
        try:
            self.stop_all()
        finally:
            self._ser.close()


class RecordingTransport:
    """Keeps every line sent; for tests and dry runs."""

    def __init__(self):
        self.lines: list[str] = []

    def send(self, line: str) -> None:
        self.lines.append(line)

    def stop_all(self) -> None:
        self.send("S")

    def close(self) -> None:
        pass


class NullTransport(RecordingTransport):
    def send(self, line: str) -> None:
        pass
