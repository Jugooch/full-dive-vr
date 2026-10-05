# SSQ — Simulator Sickness Questionnaire

- **ID:** `ssq`
- **Reference:** Kennedy, R. S., Lane, N. E., Berbaum, K. S. & Lilienthal, M. G. (1993). *Simulator Sickness Questionnaire: An enhanced method for quantifying simulator sickness.* International Journal of Aviation Psychology.
- **Context in the research:** still one of the standard VR-sickness measures. See [S42: Suitability and Comparison of Questionnaires Assessing VR-Induced Symptoms…](https://pmc.ncbi.nlm.nih.gov/articles/PMC7915458/).

Item text is not reproduced here. Use the original publication.

## Subscales

| Subscale | Item ID prefix |
|---|---|
| Nausea | `SSQ_N_` |
| Oculomotor | `SSQ_O_` |
| Disorientation | `SSQ_D_` |

Score with the original weighting from the reference publication. Record the subscale scores and the total as `ssq_nausea`, `ssq_oculomotor`, `ssq_disorientation`, `ssq_total`.

## When to administer

- Before the first block (pre-exposure baseline) and after each locomotion block.
- **Stop the session** if symptoms appear during a block. Discomfort is a result to record, not something to push through. See [safety](../../docs/safety.md).

## Response columns

`session_id,block,condition_id,item_id,response`. Use `block = 0` for the pre-exposure baseline.
