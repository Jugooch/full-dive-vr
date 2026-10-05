# Mana sensation report (custom)

- **ID:** `mana-sensation`
- **Purpose:** measure whether a sensation is perceived as **moving through the body** and as **belonging to the (virtual) body**, independently of whether the haptics actually fired. These questions come directly from the V2.1 blinded-test design in [07-version-roadmap](../../docs/research/07-version-roadmap.md) and the phantom-mana idea in [05-synthetic-physiology-mana](../../docs/research/05-synthetic-physiology-mana.md).
- **When:** after each cast or block in experiments 101–107. Answer **before** learning which haptic condition ran.

| Item | Wording | Scale |
|---|---|---|
| MS1 | "Did you feel something move?" | 0 = no, 1 = unsure, 2 = yes |
| MS2 | "Where did it start?" | body-map zone: `core`, `chest`, `shoulder_l/r`, `upper_arm_l/r`, `forearm_l/r`, `hand_l/r`, `other`, `none` |
| MS3 | "Where did it end?" | same zone list |
| MS4 | "How strong was it?" | 0–10 |
| MS5 | "Did it feel like an external vibration (0) or like something belonging to your virtual body (10)?" | 0–10 |
| MS6 | "Did it feel continuous (10) or like separate taps (0)?" | 0–10 |
| MS7 | "How strongly did the magic feel like it originated from inside your body?" | 0–10 |
| MS8 | "How powerful did the cast feel?" | 0–10 |
| MS9 | Free text: describe the sensation in your own words (tingling, static, wind-like, warmth, pressure…) | text |

## Notes

- MS9 matters for the phantom-sensation question. Phantom touch reports were often described as tingling, static or wind-like ([S74](https://pmc.ncbi.nlm.nih.gov/articles/PMC10507094/)). Don't prompt with these words.
- For visual-only catch trials, any MS1 = 2 response is the key outcome of experiment 104.

## Response columns

`session_id,block,condition_id,item_id,response`. Add a `cast` index in `block` (e.g. `3.07` = block 3, cast 7) when rating per cast.
