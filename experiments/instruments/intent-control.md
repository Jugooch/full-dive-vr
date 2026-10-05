# Intent control ratings (custom)

- **ID:** `intent-control`
- **Purpose:** subjective counterpart to the objective control metrics (accuracy, false activations/min, latency) for any biosignal-driven input.
- **When:** after each input block.

| Item | Wording | Scale |
|---|---|---|
| IC1 | "The virtual body did what I intended." | 1–7 (strongly disagree → strongly agree) |
| IC2 | "Responses felt immediate." (perceived latency) | 1–7 |
| IC3 | "How much physical effort did controlling it take?" | 0–10 (none → maximal) |
| IC4 | "Control felt automatic. I didn't have to think about how to trigger it." | 1–7 |
| IC5 | "How many times did something happen that you did NOT intend?" | count (best estimate) |
| IC6 | "How many times did you intend something that did NOT happen?" | count (best estimate) |
| IC7 | "Did it feel like moving, or like pressing a button?" | 1 (button) – 7 (moving) |
| IC8 | Free text: anything notable (fatigue, electrode discomfort, drift) | text |

Compare IC5 and IC6 with the logged false activations and missed intents. The gap between perceived and measured errors is itself informative.

## Response columns

`session_id,block,condition_id,item_id,response`
