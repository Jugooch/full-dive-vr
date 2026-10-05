// Blueprint mirrors of the contracts in /schemas (full-dive-vr repo).
// Change the schema, services/src/partialdive/contracts and this file together.
#pragma once

#include "CoreMinimal.h"
#include "PDBridgeTypes.generated.h"

/** intent-frame/1 — the ONLY body input gameplay receives. Channels are 0..1. */
USTRUCT(BlueprintType)
struct PARTIALDIVEBRIDGE_API FPDIntentFrame
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent") int64 Seq = 0;
	/** LSL clock seconds at decode time. */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent") double Timestamp = 0.0;
	/** joystick | keyboard | emg | eeg | gaze | fusion | replay | simulated */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent") FString Source;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent") float Confidence = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float WalkForward = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float StepLeft = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float StepRight = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float TurnLeft = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float TurnRight = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float GrabLeft = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float GrabRight = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Body") float ActionStrength = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") float CoreActivation = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") float RouteLeft = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") float RouteRight = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") float Release = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") bool bHasRespirationPhase = false;
	/** -1 full exhale .. +1 full inhale (valid only if bHasRespirationPhase). */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") float RespirationPhase = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent|Mana") float Stillness = 0.f;

	/** Object id the user is looking at, empty if unknown. */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent") FString GazeTarget;
	/** Experiment-specific channels not yet promoted to fields. */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Intent") TMap<FString, float> Extra;
};

/** haptic-event/1 kinds. */
UENUM(BlueprintType)
enum class EPDHapticKind : uint8
{
	Contact, Impact, Pulse, Flow, Rumble, Airflow, StopAll
};

/** haptic-event/1 zones (order must match PDBridgeJson.cpp ZoneNames). */
UENUM(BlueprintType)
enum class EPDHapticZone : uint8
{
	Core, Chest, UpperBack,
	LeftShoulder, RightShoulder,
	LeftUpperArm, RightUpperArm,
	LeftForearm, RightForearm,
	LeftHand, RightHand,
	LeftTorso, RightTorso,
	LeftThigh, RightThigh,
	LeftFoot, RightFoot,
	Seat, AirFront, AirLeft, AirRight, AirOverhead, All
};

/** block/1 — condition block start. Params are for the runtime only; never display them. */
USTRUCT(BlueprintType)
struct PARTIALDIVEBRIDGE_API FPDBlockInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Block") FString SessionId;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Block") FString Experiment;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Block") int32 Block = 0;
	/** Opaque block code — the ONLY thing the participant HUD may show. */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Block") FString Code;
	/** Raw params JSON; prefer the GetBlockParam* helpers on the subsystem. */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Block") FString ParamsJson;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Block") double Timestamp = 0.0;
};

/** chant-phrase/1 (V4). */
USTRUCT(BlueprintType)
struct PARTIALDIVEBRIDGE_API FPDChantPhrase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") double Timestamp = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") FString Spell;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") int32 PhraseIndex = 0;
	/** SENSE | GATHER | ROUTE | SHAPE | CHARGE | RELEASE */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") FString Phase;
	/** Logging only — never drive logic from recognized text. */
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") FString Text;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") float Confidence = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") float TimingErrorMs = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "PartialDive|Voice") float CadenceStability = 0.f;
};
