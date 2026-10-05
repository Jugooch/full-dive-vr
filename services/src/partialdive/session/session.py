"""Create and unblind experiment sessions.

Layout:
    data/sessions/<experiment>/<YYYY-MM-DD>_sNN/
        session.yaml        # committed: metadata, block codes, metrics, questionnaire scores
        sealed.yaml         # committed after unblinding: seed + code -> condition key
        responses.csv       # committed: questionnaire answers (session_id,block,condition_id,item_id,response)
        recording.xdf       # NOT committed (LabRecorder output) — see data/README.md
        calibration.yaml    # committed: per-channel rest/max calibration used by the decoder

While blinded, the runtime (haptic bus / Unreal) reads sealed.yaml to apply conditions; the participant-
facing display only ever shows block codes.
"""

from __future__ import annotations

import datetime as dt
import hashlib
import secrets
import subprocess
from pathlib import Path

import yaml

from ..paths import experiments_dir, hardware_profiles_dir, repo_root, sessions_dir
from .randomize import blinded_order


def _git(*args: str) -> str:
    try:
        return subprocess.run(["git", *args], cwd=repo_root(), capture_output=True, text=True,
                              check=True).stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "unknown"


def find_experiment(experiment: str) -> Path:
    """Accept '002' or '002-synchronized-touch'."""
    matches = sorted(p for p in experiments_dir().glob(f"{experiment}*") if p.is_dir())
    if len(matches) != 1:
        raise FileNotFoundError(f"experiment {experiment!r} matched {[m.name for m in matches]}")
    return matches[0]


def load_conditions(experiment: str) -> tuple[Path, dict]:
    exp_dir = find_experiment(experiment)
    with open(exp_dir / "conditions.yaml") as f:
        return exp_dir, yaml.safe_load(f)


def init_session(experiment: str, profile: str, participant: str = "P01",
                 seed: int | None = None, date: dt.date | None = None) -> Path:
    exp_dir, spec = load_conditions(experiment)
    profile_path = hardware_profiles_dir() / f"{profile}.yaml"
    if not profile_path.exists():
        raise FileNotFoundError(f"hardware profile {profile_path} not found")
    hw = yaml.safe_load(profile_path.read_text())

    date = date or dt.date.today()
    root = sessions_dir() / exp_dir.name
    n = 1 + sum(1 for p in root.glob(f"{date.isoformat()}_s*") if p.is_dir()) if root.exists() else 1
    session_id = f"{exp_dir.name}_{date.isoformat()}_s{n:02d}"
    out = root / f"{date.isoformat()}_s{n:02d}"
    out.mkdir(parents=True, exist_ok=False)

    seed = seed if seed is not None else secrets.randbits(32)
    blocks = blinded_order(spec["conditions"], int(spec.get("repetitions", 1)), seed,
                           randomize=spec.get("randomize", True))
    blinded = spec.get("blinded", True)

    session = {
        "schema": "session/1",
        "session_id": session_id,
        "experiment": exp_dir.name,
        "version": spec.get("version"),
        "participant": participant,
        "created": dt.datetime.now().astimezone().isoformat(timespec="seconds"),
        "software": {
            "repo_commit": _git("rev-parse", "--short", "HEAD"),
            "repo_dirty": bool(_git("status", "--porcelain")),
            "unreal_build": None,
            "firmware": {},
        },
        "hardware": {
            "profile": profile,
            "headset": hw.get("headset"),
            "emg": (hw.get("emg") or {}).get("device"),
            "eeg": (hw.get("eeg") or {}).get("device"),
            "haptics": (hw.get("haptics") or {}).get("device"),
        },
        "seed_hash": hashlib.sha256(str(seed).encode()).hexdigest(),
        "blocks": [
            {"block": b.block, "code": b.code,
             "condition_id": None if blinded else b.condition_id,
             "params": None if blinded else b.params}
            for b in blocks
        ],
        "measures": spec.get("measures", []),
        "metrics": {},
        "questionnaires": {},
        "notes": "",
    }
    sealed = {"seed": seed, "key": {b.code: {"condition_id": b.condition_id, "params": b.params} for b in blocks}}

    (out / "session.yaml").write_text(yaml.safe_dump(session, sort_keys=False))
    (out / "sealed.yaml").write_text(
        "# SEALED: do not open until analysis. The runtime reads this to apply conditions.\n"
        + yaml.safe_dump(sealed, sort_keys=False)
    )
    (out / "responses.csv").write_text("session_id,block,condition_id,item_id,response\n")
    return out


def unblind_session(session_dir: Path) -> dict:
    """Copy condition ids/params from sealed.yaml into session.yaml (after data collection)."""
    session_dir = Path(session_dir)
    session = yaml.safe_load((session_dir / "session.yaml").read_text())
    sealed = yaml.safe_load((session_dir / "sealed.yaml").read_text())
    if hashlib.sha256(str(sealed["seed"]).encode()).hexdigest() != session["seed_hash"]:
        raise ValueError("sealed.yaml does not match this session's seed hash")
    for block in session["blocks"]:
        entry = sealed["key"][block["code"]]
        block["condition_id"], block["params"] = entry["condition_id"], entry["params"]
    session["unblinded"] = dt.datetime.now().astimezone().isoformat(timespec="seconds")
    (session_dir / "session.yaml").write_text(yaml.safe_dump(session, sort_keys=False))
    return session
