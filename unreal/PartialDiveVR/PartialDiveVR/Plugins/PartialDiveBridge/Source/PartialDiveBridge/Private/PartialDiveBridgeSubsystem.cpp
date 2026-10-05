#include "PartialDiveBridgeSubsystem.h"

#include "Common/UdpSocketBuilder.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Dom/JsonObject.h"
#include "HAL/PlatformTime.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "PDBridgeJson.h"
#include "PartialDiveBridge.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

#include "lsl_c.h"

namespace
{
	const FIPv4Address Localhost(127, 0, 0, 1);
	constexpr int32 MaxDatagram = 65507;

	TAutoConsoleVariable<int32> CVarPDDebug(
		TEXT("pdive.debug"),
		WITH_EDITOR ? 1 : 0,
		TEXT("PartialDive bridge on-screen overlay: 0 = off, 1 = on (default on in editor builds).\n")
		TEXT("Shows intent (live/stale, rate, all channels), active block CODE (never params), last haptic event."));
}

// ------------------------------------------------------------------ lifecycle

void UPartialDiveBridgeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RecvBuffer.SetNumUninitialized(MaxDatagram);

	IntentSocket = MakeReceiver(TEXT("PD.Intent"), IntentPort);
	ControlSocket = MakeReceiver(TEXT("PD.Control"), ControlPort);
	VoiceSocket = MakeReceiver(TEXT("PD.Voice"), VoicePort);

	SendSocket = FUdpSocketBuilder(TEXT("PD.HapticOut")).AsNonBlocking().Build();
	HapticAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	HapticAddr->SetIp(Localhost.Value);
	HapticAddr->SetPort(HapticPort);

	lsl_streaminfo Info = lsl_create_streaminfo("pdive.game", "Markers", 1, LSL_IRREGULAR_RATE, cft_string, "pdive.game");
	MarkerOutlet = Info ? lsl_create_outlet(Info, 0, 360) : nullptr;
	if (Info)
	{
		lsl_destroy_streaminfo(Info);
	}

	WorldTickStartHandle = FWorldDelegates::OnWorldTickStart.AddUObject(this, &UPartialDiveBridgeSubsystem::OnWorldTickStart);
	RateWindowStart = FPlatformTime::Seconds();

	UE_LOG(LogPartialDive, Log, TEXT("PartialDiveBridge up: intent:%d control:%d voice:%d -> haptics:%d, LSL outlet %s"),
		IntentPort, ControlPort, VoicePort, HapticPort, MarkerOutlet ? TEXT("ok") : TEXT("FAILED"));
}

void UPartialDiveBridgeSubsystem::Deinitialize()
{
	FWorldDelegates::OnWorldTickStart.Remove(WorldTickStartHandle);
	StopAllHaptics();
	ISocketSubsystem* Subsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	for (FSocket** Socket : { &IntentSocket, &ControlSocket, &VoiceSocket, &SendSocket })
	{
		if (*Socket)
		{
			(*Socket)->Close();
			Subsystem->DestroySocket(*Socket);
			*Socket = nullptr;
		}
	}
	if (MarkerOutlet)
	{
		lsl_destroy_outlet(static_cast<lsl_outlet>(MarkerOutlet));
		MarkerOutlet = nullptr;
	}
	Super::Deinitialize();
}

FSocket* UPartialDiveBridgeSubsystem::MakeReceiver(const TCHAR* Name, int32 Port)
{
	FSocket* Socket = FUdpSocketBuilder(Name)
		.AsNonBlocking()
		.BoundToAddress(Localhost)
		.BoundToPort(Port)
		.WithReceiveBufferSize(1 << 20)
		.Build();
	if (!Socket)
	{
		UE_LOG(LogPartialDive, Error, TEXT("Could not bind UDP %d (%s). Is another editor/game instance running?"), Port, Name);
	}
	return Socket;
}

// ------------------------------------------------------------------ tick

ETickableTickType UPartialDiveBridgeSubsystem::GetTickableTickType() const
{
	return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always;
}

TStatId UPartialDiveBridgeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UPartialDiveBridgeSubsystem, STATGROUP_Tickables);
}

void UPartialDiveBridgeSubsystem::OnWorldTickStart(UWorld* World, ELevelTick TickType, float DeltaSeconds)
{
	// Several worlds can tick (editor + PIE); only drain for the world owned by this game instance.
	if (World && World->GetGameInstance() == GetGameInstance())
	{
		DrainAll();
	}
}

void UPartialDiveBridgeSubsystem::DrainAll()
{
	auto Handle = [this](const FString& Json) { HandleMessage(Json); };
	Drain(IntentSocket, Handle);
	Drain(ControlSocket, Handle);
	Drain(VoiceSocket, Handle);
}

void UPartialDiveBridgeSubsystem::Tick(float DeltaTime)
{
	DrainAll(); // catches anything that arrived during the frame (no-op if already drained)

	const bool bStale = IsIntentStale();
	if (bStale != bWasStale)
	{
		bWasStale = bStale;
		PushGameMarker(bStale ? TEXT("intent_stale") : TEXT("intent_resumed"), {});
		OnIntentStaleChanged.Broadcast(bStale);
	}

	const double Now = FPlatformTime::Seconds();
	if (Now - RateWindowStart >= 1.0)
	{
		IntentRateHz = FMath::RoundToInt(IntentFramesThisSecond / (Now - RateWindowStart));
		IntentFramesThisSecond = 0;
		RateWindowStart = Now;
	}

	if (CVarPDDebug.GetValueOnGameThread() != 0)
	{
		DrawDebugOverlay();
	}
}

void UPartialDiveBridgeSubsystem::DrawDebugOverlay() const
{
	if (!GEngine)
	{
		return;
	}
	// Fixed keys so lines update in place. Keys are arbitrary but unique to the bridge.
	constexpr uint64 KeyBase = 0x5044495645000000ull; // "PDIVE"
	const FPDIntentFrame& F = LatestIntent;
	const bool bStale = IsIntentStale();
	const FColor Header = bStale ? FColor::Orange : FColor::Green;
	const FColor Body = FColor(170, 230, 255);

	// AddOnScreenDebugMessage draws newest-first, so add bottom line first.
	const double HapticAge = LastHapticSentAt < 0.0 ? -1.0 : FPlatformTime::Seconds() - LastHapticSentAt;
	GEngine->AddOnScreenDebugMessage(KeyBase + 6, 0.f, Body, FString::Printf(TEXT("  haptics: %lld sent%s"),
		HapticEventsSent, LastHapticSummary.IsEmpty() ? TEXT("") :
		*FString::Printf(TEXT(" | last: %s (%.1fs ago)"), *LastHapticSummary, HapticAge)));
	GEngine->AddOnScreenDebugMessage(KeyBase + 5, 0.f, Body, bHasBlock
		? FString::Printf(TEXT("  block %d  [%s]  %s"), ActiveBlock.Block, *ActiveBlock.Code, *ActiveBlock.Experiment)
		: FString(TEXT("  block: none")));
	if (F.bHasRespirationPhase)
	{
		GEngine->AddOnScreenDebugMessage(KeyBase + 4, 0.f, Body, FString::Printf(TEXT("  respiration %.2f"), F.RespirationPhase));
	}
	else
	{
		GEngine->RemoveOnScreenDebugMessage(KeyBase + 4);
	}
	GEngine->AddOnScreenDebugMessage(KeyBase + 3, 0.f, Body, FString::Printf(
		TEXT("  mana   core %.2f   route L %.2f R %.2f   release %.2f   still %.2f"),
		F.CoreActivation, F.RouteLeft, F.RouteRight, F.Release, F.Stillness));
	GEngine->AddOnScreenDebugMessage(KeyBase + 2, 0.f, Body, FString::Printf(
		TEXT("  body   walk %.2f   step L %.0f R %.0f   turn L %.2f R %.2f   grab L %.2f R %.2f   action %.2f"),
		F.WalkForward, F.StepLeft, F.StepRight, F.TurnLeft, F.TurnRight, F.GrabLeft, F.GrabRight, F.ActionStrength));
	GEngine->AddOnScreenDebugMessage(KeyBase + 1, 0.f, Header, FString::Printf(
		TEXT("PartialDive  intent %s  %d Hz  source=%s  conf %.2f  seq %lld  input=%s   (pdive.debug 0 to hide)"),
		bStale ? TEXT("STALE") : TEXT("LIVE"), bStale ? 0 : IntentRateHz,
		F.Source.IsEmpty() ? TEXT("-") : *F.Source, F.Confidence, F.Seq,
		InputSource == EPDInputSource::Intent ? TEXT("intent") : TEXT("controller")));
}

void UPartialDiveBridgeSubsystem::Drain(FSocket* Socket, TFunctionRef<void(const FString&)> Handle)
{
	if (!Socket)
	{
		return;
	}
	uint32 Pending = 0;
	while (Socket->HasPendingData(Pending))
	{
		int32 Read = 0;
		if (!Socket->Recv(RecvBuffer.GetData(), RecvBuffer.Num(), Read) || Read <= 0)
		{
			break;
		}
		const FUTF8ToTCHAR Converted(reinterpret_cast<const ANSICHAR*>(RecvBuffer.GetData()), Read);
		Handle(FString(Converted.Length(), Converted.Get()));
	}
}

void UPartialDiveBridgeSubsystem::HandleMessage(const FString& Json)
{
	TSharedPtr<FJsonObject> Obj;
	const FString Schema = PDBridgeJson::PeekSchema(Json, Obj);

	if (Schema == TEXT("intent-frame/1"))
	{
		FPDIntentFrame Frame;
		if (PDBridgeJson::ParseIntentFrame(Obj, Frame))
		{
			LatestIntent = MoveTemp(Frame);
			LastIntentReceived = FPlatformTime::Seconds();
			++IntentFramesThisSecond;
			if (InputSource == EPDInputSource::Intent)
			{
				OnIntentFrame.Broadcast(LatestIntent);
			}
		}
	}
	else if (Schema == TEXT("block/1"))
	{
		FPDBlockInfo Block;
		if (PDBridgeJson::ParseBlock(Obj, Block))
		{
			ActiveBlock = Block;
			const TSharedPtr<FJsonObject>* Params = nullptr;
			ActiveParams = Obj->TryGetObjectField(TEXT("params"), Params) && Params ? *Params : MakeShared<FJsonObject>();
			bHasBlock = true;
			UE_LOG(LogPartialDive, Log, TEXT("Block %d started: %s"), Block.Block, *Block.Code); // never log params
			OnBlockStart.Broadcast(ActiveBlock);
		}
	}
	else if (Schema == TEXT("chant-phrase/1"))
	{
		FPDChantPhrase Phrase;
		if (PDBridgeJson::ParseChantPhrase(Obj, Phrase))
		{
			OnChantPhrase.Broadcast(Phrase);
		}
	}
	else
	{
		UE_LOG(LogPartialDive, Verbose, TEXT("Ignoring message with schema '%s'"), *Schema);
	}
}

// ------------------------------------------------------------------ intent

bool UPartialDiveBridgeSubsystem::IsIntentStale() const
{
	return LastIntentReceived < 0.0 || FPlatformTime::Seconds() - LastIntentReceived > StaleIntentSeconds;
}

FPDIntentFrame UPartialDiveBridgeSubsystem::GetIntent() const
{
	if (InputSource != EPDInputSource::Intent || IsIntentStale())
	{
		FPDIntentFrame Zero;
		Zero.Seq = LatestIntent.Seq;
		Zero.Source = LatestIntent.Source;
		return Zero;
	}
	return LatestIntent;
}

void UPartialDiveBridgeSubsystem::SetInputSource(EPDInputSource NewSource)
{
	if (NewSource != InputSource)
	{
		InputSource = NewSource;
		PushGameMarker(TEXT("input_source"), { { TEXT("source"), NewSource == EPDInputSource::Intent ? TEXT("intent") : TEXT("controller") } });
	}
}

// ------------------------------------------------------------------ haptics

int64 UPartialDiveBridgeSubsystem::EmitHaptic(EPDHapticKind Kind, EPDHapticZone Zone, float Strength, int32 DurationMs,
	const FString& Pattern, const FString& Source)
{
	if (Kind == EPDHapticKind::Flow)
	{
		UE_LOG(LogPartialDive, Warning, TEXT("EmitHaptic: use EmitHapticFlow for flow events"));
		return -1;
	}
	const int64 Id = NextHapticId++;
	SendHaptic(PDBridgeJson::BuildHapticEvent(Id, GetLslClock(), Kind, Zone, Strength, DurationMs, {}, 0, Pattern, Source));
	LastHapticSummary = FString::Printf(TEXT("#%lld %s %s %.2f %dms"), Id, *PDBridgeJson::KindName(Kind), *PDBridgeJson::ZoneName(Zone), Strength, DurationMs);
	return Id;
}

int64 UPartialDiveBridgeSubsystem::EmitHapticFlow(const TArray<EPDHapticZone>& Path, float Strength, int32 DurationMs,
	int32 StepMs, const FString& Source)
{
	if (Path.IsEmpty())
	{
		return -1;
	}
	const int64 Id = NextHapticId++;
	SendHaptic(PDBridgeJson::BuildHapticEvent(Id, GetLslClock(), EPDHapticKind::Flow, Path[0], Strength, DurationMs, Path, StepMs, FString(), Source));
	LastHapticSummary = FString::Printf(TEXT("#%lld flow %s -> %s (%d zones)"), Id, *PDBridgeJson::ZoneName(Path[0]), *PDBridgeJson::ZoneName(Path.Last()), Path.Num());
	return Id;
}

void UPartialDiveBridgeSubsystem::StopAllHaptics()
{
	SendHaptic(PDBridgeJson::BuildHapticEvent(NextHapticId++, GetLslClock(), EPDHapticKind::StopAll, EPDHapticZone::All, 0.f, 0, {}, 0, FString(), TEXT("bridge")));
}

void UPartialDiveBridgeSubsystem::SendHaptic(const FString& Json)
{
	if (!SendSocket || !HapticAddr.IsValid())
	{
		return;
	}
	const FTCHARToUTF8 Utf8(*Json);
	int32 Sent = 0;
	SendSocket->SendTo(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length(), Sent, *HapticAddr);
	++HapticEventsSent;
	LastHapticSentAt = FPlatformTime::Seconds();
}

// ------------------------------------------------------------------ block params

TSharedPtr<FJsonObject> UPartialDiveBridgeSubsystem::BlockParams() const
{
	return bHasBlock ? ActiveParams : nullptr;
}

bool UPartialDiveBridgeSubsystem::HasBlockParam(const FString& Key) const
{
	const TSharedPtr<FJsonObject> P = BlockParams();
	return P.IsValid() && P->HasField(Key);
}

FString UPartialDiveBridgeSubsystem::GetBlockParamString(const FString& Key, const FString& Default) const
{
	FString V;
	const TSharedPtr<FJsonObject> P = BlockParams();
	return P.IsValid() && P->TryGetStringField(Key, V) ? V : Default;
}

float UPartialDiveBridgeSubsystem::GetBlockParamNumber(const FString& Key, float Default) const
{
	double V = 0.0;
	const TSharedPtr<FJsonObject> P = BlockParams();
	return P.IsValid() && P->TryGetNumberField(Key, V) ? (float)V : Default;
}

bool UPartialDiveBridgeSubsystem::GetBlockParamBool(const FString& Key, bool Default) const
{
	bool V = false;
	const TSharedPtr<FJsonObject> P = BlockParams();
	return P.IsValid() && P->TryGetBoolField(Key, V) ? V : Default;
}

// ------------------------------------------------------------------ LSL

double UPartialDiveBridgeSubsystem::GetLslClock()
{
	return lsl_local_clock();
}

void UPartialDiveBridgeSubsystem::PushGameMarker(const FString& EventName, const TMap<FString, FString>& Fields)
{
	if (!MarkerOutlet)
	{
		return;
	}
	const double Now = GetLslClock();
	const FTCHARToUTF8 Utf8(*PDBridgeJson::BuildMarker(EventName, Now, Fields));
	const char* Sample[1] = { Utf8.Get() };
	lsl_push_sample_strt(static_cast<lsl_outlet>(MarkerOutlet), Sample, Now);
}
