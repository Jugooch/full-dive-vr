# VEQ — Virtual Embodiment Questionnaire

- **ID:** `veq`
- **Reference:** Roth, D. & Latoschik, M. E. (2020). *Construction of the Virtual Embodiment Questionnaire (VEQ).* IEEE TVCG. [PubMed (S51)](https://pubmed.ncbi.nlm.nih.gov/32941148/)
- **Official items and instructions:** [VEQ download page (S52)](https://sites.google.com/view/virtualembodimentquestionnaire/download-the-questionnaire)

The research program uses the VEQ as its main embodiment measure because it validates exactly the three constructs this project cares about ([04-research-program-guide](../../docs/research/04-research-program-guide.md)).

## Subscales

| Subscale | Construct | Item ID prefix |
|---|---|---|
| Ownership | "this virtual body is my body" | `VEQ_OWN_` |
| Agency | "I am the one controlling this body's movements" | `VEQ_AGE_` |
| Change | perceived change in one's own body schema | `VEQ_CHG_` |

Item text is **not** reproduced here. Use the official download, and number the items within each subscale in the order the official form gives them (`VEQ_OWN_1` … `VEQ_OWN_n`).

## Scoring

- Use the response scale given on the official form. Don't change the anchors.
- **Subscale score = mean of that subscale's items.** Don't combine the subscales into one total. The project analyses ownership, agency and change separately.
- `partialdive` analysis writes `veq_ownership`, `veq_agency` and `veq_change` into `session.yaml` → `questionnaires`.

## Response columns

`session_id,block,condition_id,item_id,response`, with `item_id` ∈ `VEQ_OWN_*`, `VEQ_AGE_*`, `VEQ_CHG_*`.

## Notes

- Ownership versus "I'm controlling a character" is the key distinction for Phase 1. Agency without ownership is a meaningful result of its own.
- Repeat it across days for baseline variance (experiment 001).
