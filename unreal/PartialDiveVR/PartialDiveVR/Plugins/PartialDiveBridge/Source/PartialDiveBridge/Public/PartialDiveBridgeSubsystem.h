// The one place Unreal talks to the partialdive Python services (contracts: /schemas).
//   in : intent-frame/1 (UDP 47800), chant-phrase/1 (UDP 47802), block/1 (UDP 47803)
//   out: haptic-event/1 (UDP 47801), LSL markers on stream "pdive.game"
// Rules: gameplay reads intent ONLY from here; Unreal never drives actuators directly;
// the participant HUD may show the block Code, never the params.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Engine/EngineBaseTypes.h"
#include "PDBridgeTypes.h"
#include "PartialDiveBridgeSubsystem.generated.h"

class FJsonObject;
class FSocket;
class FInternetAddr;

UENUM(BlueprintType)
enum class EPDInputSource : uint8
{
	/** Avatar driven by IntentFrames (EMG/EEG/fusion). */
	Intent,
	/** Conventional controller baseline (e.g. experiment 006 joystick, 103 button). */
	Controller
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDOnIntentFrame, const FPDIntentFrame&, Frame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDOnBlockStart, const FPDBlockInfo&, Block);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDOnChantPhrase, const FPDChantPhrase&, Phrase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPDOnIntentStaleChanged, bool, bIsStale);

UCLASS(Config = Game)
class PARTIALDIVEBRIDGE_API UPartialDiveBridgeSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	// ---- config (DefaultGame.ini [/Script/PartialDiveBridge.PartialDiveBridgeSubsystem]) ----
	UPROPERTY(Config) int32 IntentPort = 47800;
	UPROPERTY(Config) int32 HapticPort = 47801;
	UPROPERTY(Config) int32 VoicePort = 47802;
	UPROPERTY(Config) int32 ControlPort = 47803;
	/** No IntentFrame for this long => all intents read as 0 (avatar stops). */
	UPROPERTY(Config) float StaleIntentSeconds = 0.25f;

	// ---- events ----
	UPROPERTY(BlueprintAssignable, Category = "PartialDive") FPDOnIntentFrame OnIntentFrame;
	UPROPERTY(BlueprintAssignable, Category = "PartialDive") FPDOnBlockStart OnBlockStart;
	UPROPERTY(BlueprintAssignable, Category = "PartialDive") FPDOnChantPhrase OnChantPhrase;
	UPROPERTY(BlueprintAssignable, Category = "PartialDive") FPDOnIntentStaleChanged OnIntentStaleChanged;

	// ---- intent ----
	/** Latest frame, or an all-zero frame if intent is stale. Safe to poll every tick. */
	UFUNCTION(BlueprintPure, Category = "PartialDive|Intent") FPDIntentFrame GetIntent() const;
	UFUNCTION(BlueprintPure, Category = "PartialDive|Intent") bool IsIntentStale() const;

	UFUNCTION(BlueprintCallable, Category = "PartialDive|Intent") void SetInputSource(EPDInputSource NewSource);
	UFUNCTION(BlueprintPure, Category = "PartialDive|Intent") EPDInputSource GetInputSource() const { return InputSource; }

	// ---- haptics ----
	/** Emit the TRUE virtual contact; the haptic bus applies the experiment condition. Returns the event id. */
	UFUNCTION(BlueprintCallable, Category = "PartialDive|Haptics", meta = (AdvancedDisplay = "Pattern,Source"))
	int64 EmitHaptic(EPDHapticKind Kind, EPDHapticZone Zone, float Strength = 0.6f, int32 DurationMs = 120,
		const FString& Pattern = TEXT(""), const FString& Source = TEXT(""));

	/** Traveling sensation along Path (e.g. Core, Chest, RightShoulder, RightForearm, RightHand). */
	UFUNCTION(BlueprintCallable, Category = "PartialDive|Haptics", meta = (AdvancedDisplay = "Source"))
	int64 EmitHapticFlow(const TArray<EPDHapticZone>& Path, float Strength = 0.6f, int32 DurationMs = 80,
		int32 StepMs = 100, const FString& Source = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "PartialDive|Haptics") void StopAllHaptics();

	// ---- experiment block ----
	UFUNCTION(BlueprintPure, Category = "PartialDive|Block") bool HasActiveBlock() const { return bHasBlock; }
	UFUNCTION(BlueprintPure, Category = "PartialDive|Block") FPDBlockInfo GetActiveBlock() const { return ActiveBlock; }
	UFUNCTION(BlueprintPure, Category = "PartialDive|Block") FString GetBlockParamString(const FString& Key, const FString& Default) const;
	UFUNCTION(BlueprintPure, Category = "PartialDive|Block") float GetBlockParamNumber(const FString& Key, float Default) const;
	UFUNCTION(BlueprintPure, Category = "PartialDive|Block") bool GetBlockParamBool(const FString& Key, bool Default) const;
	UFUNCTION(BlueprintPure, Category = "PartialDive|Block") bool HasBlockParam(const FString& Key) const;

	// ---- LSL ----
	/** Push a JSON marker {"event", "t", ...Fields} on LSL stream pdive.game (trial start/end, cue, collision, cast...). */
	UFUNCTION(BlueprintCallable, Category = "PartialDive|LSL", meta = (AutoCreateRefTerm = "Fields"))
	void PushGameMarker(const FString& EventName, const TMap<FString, FString>& Fields);

	/** Seconds on the shared LSL clock. Use for every timestamp. */
	UFUNCTION(BlueprintPure, Category = "PartialDive|LSL") static double GetLslClock();

	// ---- USubsystem ----
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ---- FTickableGameObject ----
	/** Per-frame bookkeeping (stale detection, debug overlay). Network input is drained earlier, at world tick start. */
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual TStatId GetStatId() const override;

private:
	/** Drains all sockets at the START of the world tick, so every actor sees the newest intent this frame. */
	void OnWorldTickStart(UWorld* World, ELevelTick TickType, float DeltaSeconds);
	void DrainAll();
	void DrawDebugOverlay() const;

	FSocket* MakeReceiver(const TCHAR* Name, int32 Port);
	void Drain(FSocket* Socket, TFunctionRef<void(const FString&)> Handle);
	void HandleMessage(const FString& Json);
	void SendHaptic(const FString& Json);
	TSharedPtr<FJsonObject> BlockParams() const;

	FSocket* IntentSocket = nullptr;
	FSocket* ControlSocket = nullptr;
	FSocket* VoiceSocket = nullptr;
	FSocket* SendSocket = nullptr;
	TSharedPtr<FInternetAddr> HapticAddr;

	void* MarkerOutlet = nullptr; // lsl_outlet

	FPDIntentFrame LatestIntent;
	double LastIntentReceived = -1.0; // FPlatformTime::Seconds()
	bool bWasStale = true;
	EPDInputSource InputSource = EPDInputSource::Intent;

	FPDBlockInfo ActiveBlock;
	TSharedPtr<FJsonObject> ActiveParams;
	bool bHasBlock = false;

	int64 NextHapticId = 1;
	TArray<uint8> RecvBuffer;
	FDelegateHandle WorldTickStartHandle;

	// Debug overlay stats
	int32 IntentFramesThisSecond = 0;
	int32 IntentRateHz = 0;
	double RateWindowStart = 0.0;
	FString LastHapticSummary;
	double LastHapticSentAt = -1.0;
	int64 HapticEventsSent = 0;
};
