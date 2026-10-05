# Hardware

| Folder / file | Contents |
|---|---|
| [`profiles/`](profiles/) | One YAML per physical setup: sensors, placements, ports, intent mapping, zone → channel |
| [`bom.csv`](bom.csv) | Bill of materials by stage (2026 prices from the research; re-check before buying) |
| [`wiring/`](wiring/) | Wiring diagrams and photos of each build revision (LFS for images) |
| [`pod/`](pod/) | Sensory recliner / pod: mechanical design, CAD (`.step`/`.stl` via LFS), build notes |

The buying guide with rationale is [`docs/hardware.md`](../docs/hardware.md). Safety rules that constrain
every build are in [`docs/safety.md`](../docs/safety.md).

Revision convention: name builds `haptics-v1`, `recliner-v1`, … and reference the same name in the
hardware profile's `haptics.device` / `emg.device` so sessions trace to the exact build.
