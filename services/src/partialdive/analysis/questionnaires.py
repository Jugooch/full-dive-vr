"""Questionnaire scoring from the long-format responses.csv
(session_id,block,condition_id,item_id,response).

Item -> subscale maps live in experiments/instruments/*.md alongside the instrument references.
Licensed item wording is never stored in the repo; only item ids.
"""

from __future__ import annotations

import csv
from collections import defaultdict
from pathlib import Path


def subscale_scores(responses_csv: Path, item_map: dict[str, str],
                    reverse_items: set[str] | None = None, scale: tuple[int, int] = (1, 7)) -> list[dict]:
    """Mean per subscale for each (block, condition). Reverse-scored items are flipped on `scale`."""
    reverse_items = reverse_items or set()
    lo, hi = scale
    groups: dict[tuple, dict[str, list[float]]] = defaultdict(lambda: defaultdict(list))
    with open(responses_csv, newline="") as f:
        for row in csv.DictReader(f):
            item = row["item_id"]
            if item not in item_map:
                continue
            value = float(row["response"])
            if item in reverse_items:
                value = lo + hi - value
            key = (row["session_id"], int(row["block"]), row["condition_id"])
            groups[key][item_map[item]].append(value)
    return [
        {"session_id": s, "block": b, "condition_id": c,
         **{sub: sum(vals) / len(vals) for sub, vals in subs.items()}}
        for (s, b, c), subs in sorted(groups.items())
    ]
