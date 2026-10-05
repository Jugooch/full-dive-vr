# Unreal — the experience side

Engine: **Unreal Engine 5.8**, **OpenXR**, PCVR on an **Oculus Rift S** (Meta PC app as the OpenXR runtime;
no Link and no Quest needed) [S45–S48]. See [ADR 0002](../docs/decisions/0002-unreal-openxr.md).

```text
unreal/
├── README.md                     ← this file (setup)
└── PartialDiveVR/
    ├── README.md                 ← project spec + bridge usage
    └── PartialDiveVR/            ← the Unreal project
        ├── PartialDiveVR.uproject
        ├── Source/PartialDiveVR/ ← game module (C++)
        ├── Plugins/
        │   ├── PartialDiveBridge/  ← our I/O plugin (C++, see spec)
        │   └── LSL/                ← git submodule: labstreaminglayer/plugin-UE4
        ├── Config/  Content/       ← Content is Git LFS (.uasset/.umap)
        └── .vsconfig               ← Visual Studio components Unreal needs
```

## Fresh clone

```bash
git clone --recurse-submodules https://github.com/Jugooch/full-dive-vr.git
# or, in an existing clone:
git submodule update --init
git lfs install && git lfs pull
```

Then right-click `PartialDiveVR.uproject` → **Generate Visual Studio project files**, open
`PartialDiveVR.sln`, build **Development Editor | Win64** (Ctrl+Shift+B), and open the project.

## Toolchain

- **Visual Studio 2022** with *Game development with C++* + *Desktop development with C++* (exact
  components in `.vsconfig`; VS offers to install them). Use `PartialDiveVR.sln`, not
  `Automation_PartialDiveVR.sln` (that one is generated for VS 2026 / .NET 10 and isn't needed).
- Headless build (what CI or Claude uses):
  ```
  "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" PartialDiveVREditor Win64 Development -Project="<path>\PartialDiveVR.uproject" -WaitMutex
  ```
- Bridge unit tests (headless):
  ```
  UnrealEditor-Cmd.exe "<path>\PartialDiveVR.uproject" -ExecCmds="Automation RunTests PartialDive; Quit" -unattended -nullrhi -nosplash -nosound
  ```
  or in the editor: Tools → Session Frontend → Automation → filter "PartialDive".

## How this project was set up (for reference)

1. Created from **Games → Virtual Reality** (Blueprint) in UE 5.8; OpenXR enabled by the template.
2. Converted to C++ with **Tools → New C++ Class** (`PDGameTypes`), which adds `Source/` and a module entry.
3. LSL: `git submodule add https://github.com/labstreaminglayer/plugin-UE4.git Plugins/LSL`. The repo name
   says UE4, but it supports 4.26 → 5.7 and builds on 5.8. A submodule (not a copy) because `.gitignore`
   excludes plugin `Binaries/`, which would drop its `liblsl64.dll`.
4. `PartialDiveBridge` created with Edit → Plugins → + Add → Blank, then implemented (see spec).

### Rift S notes

- Meta PC app → Settings → General → **OpenXR Runtime: set as active**. Check the current app still
  fully supports the Rift S (DisplayPort + USB 3.0 connection).
- Inside-out tracking (cameras on the headset): no external sensors to place. When **reclined**, keep
  the room reasonably lit and the controllers in view of the headset cameras; do the Guardian/floor setup
  while sitting in the chair, and use a stationary boundary.
- Built-in microphone (V4 chants). No eye tracking, so Phase 6 "gaze" uses head direction instead.

### Troubleshooting

- **`Expecting to find a type ... named 'VisualStudioTools' in UE5Rules`**: the engine's precompiled rules
  cache (`Engine\Intermediate\Build\BuildRules\UE5Rules.dll`) predates a newly installed engine plugin.
  Deleting it is **not enough** on a Launcher-installed engine ("Precompiled rules assembly ... does not
  exist"). Fix: Epic Launcher → UE 5.8 → **Verify**, or regenerate the cache once by temporarily renaming
  `Engine\Build\InstalledBuild.txt`, running `UnrealBuildTool.exe -projectfiles -project=<uproject> -game`,
  and restoring the file. (Happened 2026-10-05; see research log.)
- **`Could not bind UDP 4780x`** in the log: another editor/game instance (or a Python tool) already owns
  that port. Only one PIE instance at a time.

## Rules

- **Gameplay never reads biosignals.** It reads `IntentFrame`s from the bridge, and nothing else.
- **Unreal never drives actuators.** It emits `HapticEvent`s with the *true* virtual contact; the haptic
  bus applies the experiment condition.
- **No condition logic the participant can see.** Block params arrive via `block/1`; the HUD shows only
  the block code.
- Every scene used in an experiment pushes LSL markers for trial start/end, cues and task events.
