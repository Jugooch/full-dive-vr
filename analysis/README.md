# Analysis

| Folder | Contents |
|---|---|
| `notebooks/<experiment-id>/` | Jupyter notebooks per experiment (clear outputs before committing large figures) |
| `reports/<experiment-id>.md` | Written result summaries + small figures; link from `docs/research-log.md` |

Shared metric code lives in the `partialdive.analysis` package (`services/src/partialdive/analysis/`)
so every experiment uses the same definitions ([`docs/metrics.md`](../docs/metrics.md)).

```bash
pip install -e "services[analysis,lsl]"
```

```python
import pyxdf
from partialdive.analysis import false_activations_per_min, onset_latencies, effort_ratio, subscale_scores

streams, header = pyxdf.load_xdf("data/sessions/003-emg-binary-intent/2026-11-02_s01/recording.xdf")
by_name = {s["info"]["name"][0]: s for s in streams}   # pdive.emg, pdive.intent, pdive.game, pdive.experiment ...
```

Rules: split train/test by run and by day, never by adjacent window. Report your own baseline variance
next to every effect ([`docs/research-protocol.md`](../docs/research-protocol.md)).
