"""Localhost UDP JSON transport between the Python services and Unreal.

UDP because every message is a self-contained latest-state snapshot or event; a dropped IntentFrame is
superseded ~10 ms later, and retransmission would only add latency.
"""

from __future__ import annotations

import json
import socket

DEFAULT_PORTS = {"intent": 47800, "haptic": 47801, "voice": 47802, "control": 47803}


class UdpJsonSender:
    def __init__(self, host: str = "127.0.0.1", port: int = DEFAULT_PORTS["intent"]):
        self._addr = (host, port)
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def send(self, payload: bytes | dict) -> None:
        data = payload if isinstance(payload, bytes) else json.dumps(payload).encode()
        self._sock.sendto(data, self._addr)

    def close(self) -> None:
        self._sock.close()


class UdpJsonReceiver:
    def __init__(self, port: int, host: str = "127.0.0.1"):
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._sock.bind((host, port))
        self._sock.setblocking(False)

    def poll(self, max_messages: int = 256) -> list[dict]:
        """Return all datagrams currently queued (non-blocking). Malformed ones are skipped."""
        out = []
        for _ in range(max_messages):
            try:
                data, _ = self._sock.recvfrom(65535)
            except BlockingIOError:
                break
            try:
                out.append(json.loads(data))
            except json.JSONDecodeError:
                continue
        return out

    def close(self) -> None:
        self._sock.close()
