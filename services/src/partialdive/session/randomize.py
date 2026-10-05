"""Blocked randomization with opaque block codes.

Each condition appears `repetitions` times.
- randomize: true   -> shuffled within each repetition round, so conditions stay balanced across the
                       session (no "all A's at the start" fatigue confound). Default.
- randomize: full   -> one global shuffle. Use when a block is a single trial and the next condition must
                       not be predictable (e.g. 104 visual-only catch casts).
- randomize: false  -> fixed order as listed (training progressions, e.g. 007).
"""

from __future__ import annotations

import random
import string
from dataclasses import dataclass


@dataclass
class Block:
    block: int
    code: str
    condition_id: str
    params: dict


def _code(rng: random.Random) -> str:
    return "".join(rng.choice(string.ascii_uppercase.replace("O", "").replace("I", "")) for _ in range(4))


def blinded_order(conditions: list[dict], repetitions: int, seed: int,
                  randomize: bool | str = True) -> list[Block]:
    rng = random.Random(seed)
    if randomize not in (True, False, "full"):
        raise ValueError(f"randomize must be true, false or 'full', got {randomize!r}")
    order: list[dict] = []
    for _ in range(repetitions):
        round_ = list(conditions)
        if randomize is True:
            rng.shuffle(round_)
        order.extend(round_)
    if randomize == "full":
        rng.shuffle(order)

    blocks: list[Block] = []
    used: set[str] = set()
    for cond in order:
        code = _code(rng)
        while code in used:
            code = _code(rng)
        used.add(code)
        blocks.append(Block(len(blocks) + 1, code, cond["id"], dict(cond.get("params") or {})))
    return blocks
