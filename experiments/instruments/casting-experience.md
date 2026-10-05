# Casting experience ratings (custom)

- **ID:** `casting-experience`
- **Purpose:** the outcome measures from the "embodied vs arbitrary control" comparison in [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md). The question is "which makes you feel like you actually possess the fictional ability?", not "which is the fastest UI?".
- **When:** after each casting block.

| Item | Wording | Scale |
|---|---|---|
| CE1 | "I was the one causing the magic." (agency) | 1–7 |
| CE2 | "I felt present in the virtual world." (presence) | 1–7 |
| CE3 | "The magic felt powerful." (perceived power) | 1–7 |
| CE4 | "How physically effortful was casting?" | 0–10 |
| CE5 | "I enjoyed casting this way." | 1–7 |
| CE6 | "Casting this way was easy to learn." | 1–7 |
| CE7 | "The magic originated from my body." | 1–7 |
| CE8 | "It felt like I possess this ability, rather than triggering a game effect." | 1–7 |
| CE9 | (106 and later) "The chant helped me control the mana." | 1–7; N/A if no chant |
| CE10 | Free text | text |

## Response columns

`session_id,block,condition_id,item_id,response`
