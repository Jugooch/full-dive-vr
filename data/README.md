# Data

```text
data/
├── sessions/
│   └── <experiment-id>/
│       └── <YYYY-MM-DD>_sNN/
│           ├── session.yaml       ✅ committed: metadata, block codes, metrics, questionnaire scores
│           ├── sealed.yaml        ✅ committed (open only after `partialdive session unblind`)
│           ├── calibration.yaml   ✅ committed: decoder calibration used in this session
│           ├── responses.csv      ✅ committed: session_id,block,condition_id,item_id,response
│           ├── notes.md           ✅ optional free-form notes
│           └── recording.xdf      ❌ NOT in git (LabRecorder output)
├── raw/                           ❌ ignored: anything bulky
└── scratch/                       ❌ ignored: throwaway
```

## Rules

- **Large raw recordings never go into ordinary git history** (`*.xdf`, audio, video are gitignored).
  Back them up somewhere else: an external drive, cloud storage, or a separate data repo/DVC remote. Keep the
  same relative path so analysis code can find them.
- Small, derived, human-readable files (YAML/CSV) *are* committed; they are the research record.
- Never edit a session's data after the fact. Corrections go in `notes.md` with a date.
- Participant IDs are anonymous (`P01` = you). No names, faces or voice recordings of other people in git.
- Session folders are created by `partialdive session init`, never by hand.

## Backing up XDF (suggested)

Mirror `data/sessions/` (including XDF) to a backup location after every session, e.g.
`robocopy data\sessions D:\full-dive-vr-data\sessions /E` on Windows, or `rsync -a` elsewhere.
