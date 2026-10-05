"""Repository-relative paths, so tools work from any working directory inside the repo."""

from __future__ import annotations

from pathlib import Path


def repo_root(start: Path | None = None) -> Path:
    """Walk up from `start` (default: this file) to the directory containing `.git`."""
    here = (start or Path(__file__)).resolve()
    for candidate in [here, *here.parents]:
        if (candidate / ".git").exists():
            return candidate
    raise FileNotFoundError("Not inside the full-dive-vr repository")


def experiments_dir() -> Path:
    return repo_root() / "experiments"


def sessions_dir() -> Path:
    return repo_root() / "data" / "sessions"


def hardware_profiles_dir() -> Path:
    return repo_root() / "hardware" / "profiles"
