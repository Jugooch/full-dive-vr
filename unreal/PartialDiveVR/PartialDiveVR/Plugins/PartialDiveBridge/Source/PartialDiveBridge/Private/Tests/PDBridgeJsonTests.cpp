// Run: Session Frontend > Automation > "PartialDive", or
//   UnrealEditor-Cmd <uproject> -ExecCmds="Automation RunTests PartialDive; Quit" -unattended -nullrhi -nosplash
#include "Dom/JsonObject.h"
#include "Misc/AutomationTest.h"
#include "PDBridgeJson.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPDIntentParseTest, "PartialDive.Json.IntentFrame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPDIntentParseTest::RunTest(const FString& Parameters)
{
	// Exactly what partialdive's IntentFrame.to_json() sends: zero channels omitted.
	const FString Json = TEXT("{\"schema\":\"intent-frame/1\",\"seq\":42,\"timestamp\":12.5,\"source\":\"emg\",\"confidence\":1.0,")
		TEXT("\"grab_right\":1.0,\"walk_forward\":0.625,\"core_activation\":0.5,\"respiration_phase\":-0.5,\"extra\":{\"x\":0.25}}");
	TSharedPtr<FJsonObject> Obj;
	TestEqual(TEXT("schema"), PDBridgeJson::PeekSchema(Json, Obj), FString(TEXT("intent-frame/1")));

	FPDIntentFrame F;
	TestTrue(TEXT("parsed"), PDBridgeJson::ParseIntentFrame(Obj, F));
	TestEqual(TEXT("seq"), F.Seq, (int64)42);
	TestEqual(TEXT("source"), F.Source, FString(TEXT("emg")));
	TestEqual(TEXT("grab_right"), F.GrabRight, 1.f);
	TestEqual(TEXT("walk_forward"), F.WalkForward, 0.625f);
	TestEqual(TEXT("omitted channel is zero"), F.GrabLeft, 0.f);
	TestEqual(TEXT("core"), F.CoreActivation, 0.5f);
	TestTrue(TEXT("has respiration"), F.bHasRespirationPhase);
	TestEqual(TEXT("respiration"), F.RespirationPhase, -0.5f);
	TestEqual(TEXT("extra"), F.Extra.FindRef(TEXT("x")), 0.25f);

	TSharedPtr<FJsonObject> Other;
	PDBridgeJson::PeekSchema(TEXT("{\"schema\":\"block/1\"}"), Other);
	TestFalse(TEXT("wrong schema rejected"), PDBridgeJson::ParseIntentFrame(Other, F));
	TestEqual(TEXT("garbage"), PDBridgeJson::PeekSchema(TEXT("not json"), Other), FString());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPDBlockParseTest, "PartialDive.Json.Block",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPDBlockParseTest::RunTest(const FString& Parameters)
{
	const FString Json = TEXT("{\"schema\":\"block/1\",\"timestamp\":3.0,\"session_id\":\"002-x_2026-10-05_s01\",")
		TEXT("\"experiment\":\"002-synchronized-touch\",\"block\":2,\"code\":\"KPET\",\"params\":{\"haptic_delay_ms\":150,\"mirror\":true}}");
	TSharedPtr<FJsonObject> Obj;
	PDBridgeJson::PeekSchema(Json, Obj);
	FPDBlockInfo B;
	TestTrue(TEXT("parsed"), PDBridgeJson::ParseBlock(Obj, B));
	TestEqual(TEXT("code"), B.Code, FString(TEXT("KPET")));
	TestEqual(TEXT("block"), B.Block, 2);
	TestTrue(TEXT("params kept"), B.ParamsJson.Contains(TEXT("haptic_delay_ms")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPDHapticBuildTest, "PartialDive.Json.HapticEvent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPDHapticBuildTest::RunTest(const FString& Parameters)
{
	const FString Contact = PDBridgeJson::BuildHapticEvent(7, 1.5, EPDHapticKind::Contact, EPDHapticZone::RightForearm,
		0.6f, 120, {}, 0, FString(), TEXT("collision"));
	TSharedPtr<FJsonObject> Obj;
	TestEqual(TEXT("schema"), PDBridgeJson::PeekSchema(Contact, Obj), FString(TEXT("haptic-event/1")));
	TestEqual(TEXT("zone"), Obj->GetStringField(TEXT("zone")), FString(TEXT("right_forearm")));
	TestEqual(TEXT("kind"), Obj->GetStringField(TEXT("kind")), FString(TEXT("contact")));
	TestFalse(TEXT("no path on contact"), Obj->HasField(TEXT("path")));
	TestTrue(TEXT("integer id"), Contact.Contains(TEXT("\"id\":7,")));
	TestTrue(TEXT("integer duration"), Contact.Contains(TEXT("\"duration_ms\":120")));

	const FString Flow = PDBridgeJson::BuildHapticEvent(8, 2.0, EPDHapticKind::Flow, EPDHapticZone::Core, 0.5f, 80,
		{ EPDHapticZone::Core, EPDHapticZone::Chest, EPDHapticZone::RightHand }, 100, FString(), FString());
	PDBridgeJson::PeekSchema(Flow, Obj);
	TestEqual(TEXT("flow path"), Obj->GetArrayField(TEXT("path")).Num(), 3);
	TestEqual(TEXT("step"), (int32)Obj->GetNumberField(TEXT("step_ms")), 100);
	TestEqual(TEXT("zone names"), PDBridgeJson::ZoneName(EPDHapticZone::AirOverhead), FString(TEXT("air_overhead")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
