#include "PDBridgeJson.h"

#include "Dom/JsonObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
	// Order must match EPDHapticZone / EPDHapticKind.
	const TCHAR* ZoneNames[] = {
		TEXT("core"), TEXT("chest"), TEXT("upper_back"),
		TEXT("left_shoulder"), TEXT("right_shoulder"),
		TEXT("left_upper_arm"), TEXT("right_upper_arm"),
		TEXT("left_forearm"), TEXT("right_forearm"),
		TEXT("left_hand"), TEXT("right_hand"),
		TEXT("left_torso"), TEXT("right_torso"),
		TEXT("left_thigh"), TEXT("right_thigh"),
		TEXT("left_foot"), TEXT("right_foot"),
		TEXT("seat"), TEXT("air_front"), TEXT("air_left"), TEXT("air_right"), TEXT("air_overhead"), TEXT("all"),
	};
	static_assert(UE_ARRAY_COUNT(ZoneNames) == (int32)EPDHapticZone::All + 1, "ZoneNames out of sync with EPDHapticZone");

	const TCHAR* KindNames[] = {
		TEXT("contact"), TEXT("impact"), TEXT("pulse"), TEXT("flow"), TEXT("rumble"), TEXT("airflow"), TEXT("stop_all"),
	};
	static_assert(UE_ARRAY_COUNT(KindNames) == (int32)EPDHapticKind::StopAll + 1, "KindNames out of sync with EPDHapticKind");

	float Num(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field)
	{
		double V = 0.0;
		Obj->TryGetNumberField(Field, V);
		return (float)V;
	}

	FString Str(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field)
	{
		FString V;
		Obj->TryGetStringField(Field, V);
		return V;
	}

	bool HasSchema(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Schema)
	{
		return Obj.IsValid() && Str(Obj, TEXT("schema")) == Schema;
	}

	using FWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
	using FWriterFactory = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;
}

FString PDBridgeJson::ZoneName(EPDHapticZone Zone) { return ZoneNames[(int32)Zone]; }
FString PDBridgeJson::KindName(EPDHapticKind Kind) { return KindNames[(int32)Kind]; }

FString PDBridgeJson::PeekSchema(const FString& Json, TSharedPtr<FJsonObject>& OutObject)
{
	OutObject.Reset();
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
	if (!FJsonSerializer::Deserialize(Reader, OutObject) || !OutObject.IsValid())
	{
		return FString();
	}
	return Str(OutObject, TEXT("schema"));
}

bool PDBridgeJson::ParseIntentFrame(const TSharedPtr<FJsonObject>& Obj, FPDIntentFrame& Out)
{
	if (!HasSchema(Obj, TEXT("intent-frame/1")))
	{
		return false;
	}
	Out = FPDIntentFrame();
	double Seq = 0.0;
	Obj->TryGetNumberField(TEXT("seq"), Seq);
	Out.Seq = (int64)Seq;
	Obj->TryGetNumberField(TEXT("timestamp"), Out.Timestamp);
	Out.Source = Str(Obj, TEXT("source"));
	Out.Confidence = Num(Obj, TEXT("confidence"));

	Out.WalkForward = Num(Obj, TEXT("walk_forward"));
	Out.StepLeft = Num(Obj, TEXT("step_left"));
	Out.StepRight = Num(Obj, TEXT("step_right"));
	Out.TurnLeft = Num(Obj, TEXT("turn_left"));
	Out.TurnRight = Num(Obj, TEXT("turn_right"));
	Out.GrabLeft = Num(Obj, TEXT("grab_left"));
	Out.GrabRight = Num(Obj, TEXT("grab_right"));
	Out.ActionStrength = Num(Obj, TEXT("action_strength"));

	Out.CoreActivation = Num(Obj, TEXT("core_activation"));
	Out.RouteLeft = Num(Obj, TEXT("route_left"));
	Out.RouteRight = Num(Obj, TEXT("route_right"));
	Out.Release = Num(Obj, TEXT("release"));
	double Resp = 0.0;
	Out.bHasRespirationPhase = Obj->TryGetNumberField(TEXT("respiration_phase"), Resp);
	Out.RespirationPhase = (float)Resp;
	Out.Stillness = Num(Obj, TEXT("stillness"));

	Out.GazeTarget = Str(Obj, TEXT("gaze_target"));
	const TSharedPtr<FJsonObject>* Extra = nullptr;
	if (Obj->TryGetObjectField(TEXT("extra"), Extra) && Extra)
	{
		for (const auto& Pair : (*Extra)->Values)
		{
			double V = 0.0;
			if (Pair.Value.IsValid() && Pair.Value->TryGetNumber(V))
			{
				Out.Extra.Add(FString(Pair.Key), (float)V);
			}
		}
	}
	return true;
}

bool PDBridgeJson::ParseBlock(const TSharedPtr<FJsonObject>& Obj, FPDBlockInfo& Out)
{
	if (!HasSchema(Obj, TEXT("block/1")))
	{
		return false;
	}
	Out = FPDBlockInfo();
	Out.SessionId = Str(Obj, TEXT("session_id"));
	Out.Experiment = Str(Obj, TEXT("experiment"));
	Out.Code = Str(Obj, TEXT("code"));
	double Block = 0.0;
	Obj->TryGetNumberField(TEXT("block"), Block);
	Out.Block = (int32)Block;
	Obj->TryGetNumberField(TEXT("timestamp"), Out.Timestamp);

	const TSharedPtr<FJsonObject>* Params = nullptr;
	TSharedPtr<FJsonObject> Empty = MakeShared<FJsonObject>();
	if (!Obj->TryGetObjectField(TEXT("params"), Params) || !Params)
	{
		Params = &Empty;
	}
	const TSharedRef<FWriter> Writer = FWriterFactory::Create(&Out.ParamsJson);
	FJsonSerializer::Serialize(Params->ToSharedRef(), Writer);
	return true;
}

bool PDBridgeJson::ParseChantPhrase(const TSharedPtr<FJsonObject>& Obj, FPDChantPhrase& Out)
{
	if (!HasSchema(Obj, TEXT("chant-phrase/1")))
	{
		return false;
	}
	Out = FPDChantPhrase();
	Obj->TryGetNumberField(TEXT("timestamp"), Out.Timestamp);
	Out.Spell = Str(Obj, TEXT("spell"));
	Out.PhraseIndex = (int32)Num(Obj, TEXT("phrase_index"));
	Out.Phase = Str(Obj, TEXT("phase"));
	Out.Text = Str(Obj, TEXT("text"));
	Out.Confidence = Num(Obj, TEXT("confidence"));
	Out.TimingErrorMs = Num(Obj, TEXT("timing_error_ms"));
	Out.CadenceStability = Num(Obj, TEXT("cadence_stability"));
	return true;
}

FString PDBridgeJson::BuildHapticEvent(int64 Id, double Timestamp, EPDHapticKind Kind, EPDHapticZone Zone,
	float Strength, int32 DurationMs, const TArray<EPDHapticZone>& Path, int32 StepMs,
	const FString& Pattern, const FString& Source)
{
	FString Out;
	const TSharedRef<FWriter> W = FWriterFactory::Create(&Out);
	W->WriteObjectStart();
	W->WriteValue(TEXT("schema"), TEXT("haptic-event/1"));
	W->WriteValue(TEXT("id"), Id);
	W->WriteValue(TEXT("timestamp"), Timestamp);
	W->WriteValue(TEXT("kind"), KindName(Kind));
	W->WriteValue(TEXT("zone"), ZoneName(Zone));
	W->WriteValue(TEXT("strength"), (double)FMath::Clamp(Strength, 0.f, 1.f));
	W->WriteValue(TEXT("duration_ms"), FMath::Clamp(DurationMs, 0, 10000));
	if (Kind == EPDHapticKind::Flow)
	{
		W->WriteArrayStart(TEXT("path"));
		for (EPDHapticZone Z : Path)
		{
			W->WriteValue(ZoneName(Z));
		}
		W->WriteArrayEnd();
		W->WriteValue(TEXT("step_ms"), FMath::Max(StepMs, 1));
	}
	if (!Pattern.IsEmpty())
	{
		W->WriteValue(TEXT("pattern"), Pattern);
	}
	if (!Source.IsEmpty())
	{
		W->WriteValue(TEXT("source"), Source);
	}
	W->WriteObjectEnd();
	W->Close();
	return Out;
}

FString PDBridgeJson::BuildMarker(const FString& EventName, double Timestamp, const TMap<FString, FString>& Fields)
{
	FString Out;
	const TSharedRef<FWriter> W = FWriterFactory::Create(&Out);
	W->WriteObjectStart();
	W->WriteValue(TEXT("event"), EventName);
	W->WriteValue(TEXT("t"), Timestamp);
	for (const auto& Pair : Fields)
	{
		W->WriteValue(Pair.Key, Pair.Value);
	}
	W->WriteObjectEnd();
	W->Close();
	return Out;
}
