# Safety

This project experiments on a human body (yours). These boundaries come straight from the research
([research/03](research/03-diy-research-strategy.md), [research/04](research/04-research-program-guide.md))
and are **not** negotiable within this repo. Changing one requires an ADR written *before* any build.

## The boundary: read-only sensing + physical feedback

**In scope:** EEG *recording*, EMG recording, eye tracking, IMUs, mechanical haptics, vibration, audio,
airflow, body-position illusions, VR sensory synchronization.

**Out of scope, never DIY:**
- Electrical brain stimulation (tDCS, tACS, TMS, anything through the head)
- Peripheral nerve stimulation (including repurposed TENS/EMS for "touch")
- Galvanic vestibular stimulation (GVS), however tempting for motion
- Implants of any kind
- Body restraints, motorized limb immobilization
- Attempts to induce sleep, or to inhibit movement neurologically or pharmacologically

Once you deliberately put current through the head or nervous system you're in a different safety
category. There is a huge amount of unexplored territory on the read-only + physical-feedback side first.

<a id="electrical"></a>
## Electrical

- **Biosignal electronics on a body are never mains-coupled.** MyoWare sensors run from the battery Power
  Shield. The EMG ESP32 connects to a laptop **running on battery (charger unplugged)** or through a **USB
  isolator**. Same for EEG: OpenBCI specifies battery-only operation for Cyton [S60], and PiEEG warns it is
  not a medical device and needs complete battery isolation from mains [S44].
- No consumer device here is a medical device. Nothing is used for diagnosis or treatment.
- Fans, bass-shaker amps and other mains-powered actuators are physically separate from anything with
  electrodes, and share no ground path with the biosignal chain.
- Place electrodes per the manufacturer's guide. Don't improvise around sensitive locations. Stop on skin
  irritation.

## Mechanical / the pod

- **Support, not restraint.** The chair supports the body so it can relax; it never traps it.
- **Mechanical safety fails open.** There is always an immediate physical way to exit. No latches, clamps
  or straps that need power, a tool or a second person to release.
- Moving or pressing actuators (later pressure pads) are force- and travel-limited **in hardware**, not just in software.
- Cables are routed so they can't wrap a limb or the neck.

## Haptics and thermal

- Firmware caps every vibration pulse (2 s) and stops everything after 5 s without host commands; `S` stops all.
- Prefer **pressure, vibration and airflow before temperature.** Thermal systems introduce burn and
  cold-injury failure modes surprisingly quickly. No thermal actuator without its own ADR, hardware
  over-temperature cutoff and a log review.
- Bass shaker volume is capped at the amp; start low.

## Synthetic physiology (mana)

- **No breath-holding or hyperventilation** in any mechanic. Normal, comfortable breathing must always be
  enough to succeed.
- Core activation means *slight* activation, not crunches. If a mechanic rewards strain, redesign it.
- Voice/chant mechanics must not reward shouting or prolonged vocal strain.

## VR comfort

- Reclined, seated play area; the headset cable/strap can't snag the chair.
- Track cybersickness with the SSQ in locomotion and ablation experiments; stop if it climbs.
- Keep early sessions short (≈ 10–20 min); build up.
- Avoid conflicting motion cues for now: no vestibular manipulation (see above).

## Stopping rules (any one → stop and log)

Nausea or dizziness, headache, skin irritation or burning under an electrode or actuator, numbness, muscle
pain from sustained activation, an actuator that won't stop, or simply not wanting to continue.

## Other participants

If anyone other than you ever takes part: adults only, written informed consent describing exactly what
the hardware does, freedom to stop at any time without explanation, and their data stored under an anonymous ID
(`P02`, …). Blinding conceals *condition order*, never what the equipment can do.
