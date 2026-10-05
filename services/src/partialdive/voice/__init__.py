"""Voice / incantation service (V4, experiment 106). Intentionally empty until V4 starts.

Planned pipeline (docs/architecture.md#voice):
    headset mic -> VAD -> local recognizer (Picovoice Rhino speech-to-intent, or whisper.cpp)
    -> phrase alignment against the chant grammar -> ChantPhrase (schemas/chant-phrase.schema.json)
    -> UDP 47802 + LSL markers `pdive.voice`

Keep pronunciation forgiving: accents, mic quality and recognizer errors must not become gameplay
disadvantages. Choose the engine with an ADR when V4 begins.
"""
