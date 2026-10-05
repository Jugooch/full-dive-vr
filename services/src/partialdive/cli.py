"""`partialdive` command line. Run `partialdive -h` or `partialdive <command> -h`."""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

import numpy as np
import yaml

from .paths import hardware_profiles_dir


def _profile(name: str) -> dict:
    return yaml.safe_load((hardware_profiles_dir() / f"{name}.yaml").read_text())


def _emg_source(profile: dict, simulate: bool):
    from .biosignal import SerialEMGSource, SimulatedEMGSource

    names = [c["name"] for c in profile["emg"]["channels"]]
    if simulate:
        return names, SimulatedEMGSource(len(names), rate_hz=profile["emg"].get("sample_rate_hz", 500), realtime=True)
    return names, SerialEMGSource(profile["emg"]["port"], len(names))


# ---------------------------------------------------------------- session
def cmd_session_init(a) -> None:
    from .session import init_session

    out = init_session(a.experiment, a.profile, a.participant, a.seed)
    print(f"created {out}")
    print("block codes (run in this order):")
    for b in yaml.safe_load((out / "session.yaml").read_text())["blocks"]:
        print(f"  {b['block']:>2}  {b['code']}")


def cmd_session_unblind(a) -> None:
    from .session import unblind_session

    s = unblind_session(Path(a.session_dir))
    for b in s["blocks"]:
        print(f"  {b['block']:>2}  {b['code']}  -> {b['condition_id']}")


def _block_message(session_dir: Path, block: int) -> dict:
    from .lsl import local_clock

    session = yaml.safe_load((session_dir / "session.yaml").read_text())
    sealed = yaml.safe_load((session_dir / "sealed.yaml").read_text())
    code = session["blocks"][block - 1]["code"]
    return {"schema": "block/1", "timestamp": local_clock(), "session_id": session["session_id"],
            "experiment": session["experiment"], "block": block, "code": code,
            "params": sealed["key"][code]["params"]}


def cmd_block_start(a) -> None:
    from .lsl import MarkerOutlet
    from .net import DEFAULT_PORTS, UdpJsonSender

    msg = _block_message(Path(a.session_dir), a.block)
    marker = MarkerOutlet("pdive.experiment")
    time.sleep(0.5 if marker._outlet else 0)  # give LabRecorder a moment to see a fresh outlet
    marker.push(msg, msg["timestamp"])
    for port in (DEFAULT_PORTS["control"], DEFAULT_PORTS["haptic"]):
        UdpJsonSender(port=port).send(msg)
    print(f"block {msg['block']} started: {msg['code']}")  # never print params (blinding)


# ---------------------------------------------------------------- calibration
def _collect(source_iter, seconds: float) -> np.ndarray:
    end, rows = None, []
    for s in source_iter:
        end = end or s.timestamp + seconds
        rows.append(s.values)
        if s.timestamp >= end:
            break
    return np.asarray(rows)


def cmd_calibrate(a) -> None:
    from .intent.decoder import Calibration
    from .intent.threshold import RestCalibration

    profile = _profile(a.profile)
    names, source = _emg_source(profile, a.simulate)
    it = iter(source)
    input(f"RELAX completely for {a.seconds:.0f} s. Press Enter to start...")
    rest = _collect(it, a.seconds)
    cal = Calibration()
    for i, name in enumerate(names):
        input(f"Comfortable deliberate contraction of [{name}] for {a.seconds:.0f} s "
              "(not maximal effort). Press Enter...")
        active = _collect(it, a.seconds)
        cal.channels[name] = RestCalibration.fit(rest[:, i], active[:, i])
    Path(a.out).write_text(yaml.safe_dump(cal.to_dict(), sort_keys=False))
    print(f"wrote {a.out}")


# ---------------------------------------------------------------- decode loop
def cmd_decode(a) -> None:
    from .intent.decoder import Calibration, EmgIntentDecoder
    from .lsl import MarkerOutlet, SignalOutlet
    from .net import UdpJsonSender

    profile = _profile(a.profile)
    names, source = _emg_source(profile, a.simulate)
    cal = Calibration.from_dict(yaml.safe_load(Path(a.calibration).read_text()))
    decoder = EmgIntentDecoder(names, profile["intent_mapping"], cal)
    udp = UdpJsonSender(port=a.port)
    raw_out = SignalOutlet("pdive.emg", "EMG", names, profile["emg"].get("sample_rate_hz", 500))
    intent_out = MarkerOutlet("pdive.intent")
    period, last_sent = 1.0 / a.rate, 0.0
    print(f"decoding {names} -> udp:{a.port} at {a.rate} Hz (Ctrl+C to stop)")
    try:
        for s in source:
            raw_out.push(s.values, s.timestamp)
            frame = decoder.decode(s.timestamp, s.values)
            if s.timestamp - last_sent >= period:
                payload = frame.to_json()
                udp.send(payload)
                intent_out.push(payload.decode(), s.timestamp)
                last_sent = s.timestamp
    except KeyboardInterrupt:
        pass


# ---------------------------------------------------------------- haptics
def _haptic_bus(profile: dict, dry_run: bool):
    from .haptics import HapticBus, RecordingTransport, SerialHapticTransport
    from .lsl import MarkerOutlet

    h = profile["haptics"]
    transport = RecordingTransport() if dry_run else SerialHapticTransport(h["port"])
    markers = MarkerOutlet("pdive.haptic")
    bus = HapticBus(transport, h.get("zones", {}), h.get("fans", {}),
                    on_dispatch=lambda cmd, now: markers.push(
                        {"event": cmd.event_id, "line": cmd.line, "lag_ms": round((now - cmd.due) * 1000, 2)}, now))
    return bus, transport


def cmd_haptic_bus(a) -> None:
    from .contracts import HapticEvent
    from .haptics import HapticCondition
    from .lsl import local_clock
    from .net import UdpJsonReceiver

    bus, transport = _haptic_bus(_profile(a.profile), a.dry_run)
    if a.session_dir:
        msg = _block_message(Path(a.session_dir), a.block)
        bus.set_condition(HapticCondition.from_params(msg["params"]))
        print(f"block {a.block} ({msg['code']}) condition applied (blinded)")
    rx = UdpJsonReceiver(a.port)
    print(f"haptic bus listening on udp:{a.port} (Ctrl+C to stop)")
    try:
        while True:
            now = local_clock()
            for msg in rx.poll():
                if msg.get("schema") == "block/1":
                    bus.handle(HapticEvent(0, now, "stop_all", "all", 0.0), now)
                    bus.set_condition(HapticCondition.from_params(msg.get("params")))
                    print(f"block {msg.get('block')} ({msg.get('code')}) condition applied (blinded)")
                    continue
                try:
                    bus.handle(HapticEvent.from_dict(msg), now)
                except (ValueError, TypeError) as e:
                    print(f"rejected event: {e}", file=sys.stderr)
            bus.tick(now)
            time.sleep(0.0005)
    except KeyboardInterrupt:
        pass
    finally:
        transport.close()


def cmd_haptic_test(a) -> None:
    from .contracts import HapticEvent
    from .lsl import local_clock

    bus, transport = _haptic_bus(_profile(a.profile), a.dry_run)
    now = local_clock()
    bus.handle(HapticEvent(0, now, "contact", a.zone, a.strength, a.duration), now)
    bus.tick(now + 1)
    if a.dry_run:
        print("\n".join(transport.lines) or f"nothing sent: {bus.dropped}")
    transport.close()


def main(argv: list[str] | None = None) -> None:
    p = argparse.ArgumentParser(prog="partialdive", description=__doc__)
    sub = p.add_subparsers(required=True)

    s = sub.add_parser("session", help="create / unblind experiment sessions").add_subparsers(required=True)
    si = s.add_parser("init", help="new session with randomized blinded condition order")
    si.add_argument("experiment", help="id or prefix, e.g. 002")
    si.add_argument("--profile", required=True, help="hardware/profiles/<name>.yaml")
    si.add_argument("--participant", default="P01")
    si.add_argument("--seed", type=int)
    si.set_defaults(fn=cmd_session_init)
    su = s.add_parser("unblind", help="reveal conditions after data collection")
    su.add_argument("session_dir")
    su.set_defaults(fn=cmd_session_unblind)

    b = sub.add_parser("block", help="condition blocks").add_subparsers(required=True)
    bs = b.add_parser("start", help="send block params to Unreal + haptic bus, mark in LSL")
    bs.add_argument("session_dir")
    bs.add_argument("block", type=int)
    bs.set_defaults(fn=cmd_block_start)

    c = sub.add_parser("calibrate", help="record rest + contraction per EMG channel")
    c.add_argument("--profile", required=True)
    c.add_argument("--out", required=True, help="e.g. data/sessions/.../calibration.yaml")
    c.add_argument("--seconds", type=float, default=10.0)
    c.add_argument("--simulate", action="store_true")
    c.set_defaults(fn=cmd_calibrate)

    d = sub.add_parser("decode", help="EMG -> IntentFrame -> Unreal (UDP) + LSL")
    d.add_argument("--profile", required=True)
    d.add_argument("--calibration", required=True)
    d.add_argument("--rate", type=float, default=100.0, help="IntentFrame send rate (Hz)")
    d.add_argument("--port", type=int, default=47800)
    d.add_argument("--simulate", action="store_true")
    d.set_defaults(fn=cmd_decode)

    hb = sub.add_parser("haptic-bus", help="Unreal HapticEvents -> condition -> actuators")
    hb.add_argument("--profile", required=True)
    hb.add_argument("--session-dir")
    hb.add_argument("--block", type=int, default=1)
    hb.add_argument("--port", type=int, default=47801)
    hb.add_argument("--dry-run", action="store_true")
    hb.set_defaults(fn=cmd_haptic_bus)

    ht = sub.add_parser("haptic-test", help="fire one zone to check wiring")
    ht.add_argument("zone")
    ht.add_argument("--profile", required=True)
    ht.add_argument("--strength", type=float, default=0.6)
    ht.add_argument("--duration", type=int, default=120)
    ht.add_argument("--dry-run", action="store_true")
    ht.set_defaults(fn=cmd_haptic_test)

    args = p.parse_args(argv)
    args.fn(args)


if __name__ == "__main__":
    main()
