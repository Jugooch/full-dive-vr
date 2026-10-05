// Pure (socket-free) encode/decode of the /schemas contracts, so it can be unit tested.
#pragma once

#include "CoreMinimal.h"
#include "PDBridgeTypes.h"

class FJsonObject;

namespace PDBridgeJson
{
	/** Schema string for a zone/kind, e.g. RightForearm -> "right_forearm". */
	PARTIALDIVEBRIDGE_API FString ZoneName(EPDHapticZone Zone);
	PARTIALDIVEBRIDGE_API FString KindName(EPDHapticKind Kind);

	/** Returns the message's "schema" field ("" if not JSON / missing). */
	PARTIALDIVEBRIDGE_API FString PeekSchema(const FString& Json, TSharedPtr<FJsonObject>& OutObject);

	/** intent-frame/1. Missing channels are 0 (the Python side omits zero channels). */
	PARTIALDIVEBRIDGE_API bool ParseIntentFrame(const TSharedPtr<FJsonObject>& Obj, FPDIntentFrame& Out);
	PARTIALDIVEBRIDGE_API bool ParseBlock(const TSharedPtr<FJsonObject>& Obj, FPDBlockInfo& Out);
	PARTIALDIVEBRIDGE_API bool ParseChantPhrase(const TSharedPtr<FJsonObject>& Obj, FPDChantPhrase& Out);

	/** haptic-event/1. Path/StepMs only written for Flow; empty Pattern/Source omitted. */
	PARTIALDIVEBRIDGE_API FString BuildHapticEvent(int64 Id, double Timestamp, EPDHapticKind Kind, EPDHapticZone Zone,
		float Strength, int32 DurationMs, const TArray<EPDHapticZone>& Path, int32 StepMs,
		const FString& Pattern, const FString& Source);

	/** LSL pdive.game marker payload: {"event": Name, "t": Timestamp, ...Fields}. */
	PARTIALDIVEBRIDGE_API FString BuildMarker(const FString& EventName, double Timestamp, const TMap<FString, FString>& Fields);
}
