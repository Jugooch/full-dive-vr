// Console commands for testing without hardware or clicking through the editor.
//   pdive.view fp|front|back|hand   avatar camera mode (V cycles in game)
//   pdive.recenter                  make the current head pose straight-ahead (R in game)
//   pdive.avatar.grab <L> <R>       force hand closure 0..1 (-1 = back to real input)
//   pdive.input intent|controller   switch the bridge input source (I in game)
//   pdive.avatar.touchtest <zone>   drop a small ball through a body zone (e.g. right_forearm) -> HapticEvent
//   pdive.after <seconds> <cmd...>  run a console command later (scripted captures/tests)
#include "Containers/Ticker.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Components/CapsuleComponent.h"
#include "PDAvatar.h"
#include "PDLabProps.h"
#include "PartialDiveBridge.h"
#include "PartialDiveBridgeSubsystem.h"

namespace
{
	APDAvatar* FindAvatar(UWorld* World)
	{
		for (TActorIterator<APDAvatar> It(World); It; ++It)
		{
			return *It;
		}
		UE_LOG(LogPartialDive, Warning, TEXT("No APDAvatar in this world"));
		return nullptr;
	}

	FAutoConsoleCommandWithWorldAndArgs ViewCmd(TEXT("pdive.view"), TEXT("Avatar camera: fp | front | back | hand"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (APDAvatar* A = FindAvatar(World))
			{
				const FString M = Args.Num() ? Args[0].ToLower() : TEXT("fp");
				A->SetViewMode(M == TEXT("front") ? EPDViewMode::ThirdPersonFront : M == TEXT("back") ? EPDViewMode::ThirdPersonBack
					: M == TEXT("hand") ? EPDViewMode::HandCloseUp : EPDViewMode::FirstPerson);
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs RecenterCmd(TEXT("pdive.recenter"), TEXT("Recenter the avatar view on the current head pose"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (APDAvatar* A = FindAvatar(World)) A->Recenter();
		}));

	FAutoConsoleCommandWithWorldAndArgs GrabCmd(TEXT("pdive.avatar.grab"), TEXT("Force hand closure: <left 0..1> <right 0..1>; -1 restores real input"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (APDAvatar* A = FindAvatar(World))
			{
				A->SetDebugGrab(Args.Num() > 0 ? FCString::Atof(*Args[0]) : -1.f, Args.Num() > 1 ? FCString::Atof(*Args[1]) : -1.f);
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs HandStatsCmd(TEXT("pdive.avatar.handstats"), TEXT("Log fingertip geometry for both hands"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (APDAvatar* A = FindAvatar(World)) { A->LogHandStats(false); A->LogHandStats(true); }
		}));

	FAutoConsoleCommandWithWorldAndArgs TouchTestCmd(TEXT("pdive.avatar.touchtest"),
		TEXT("Drop a ball through a touch zone: pdive.avatar.touchtest right_forearm (zone names as in haptic-event/1)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			APDAvatar* A = FindAvatar(World);
			if (!A || !World)
			{
				return;
			}
			const FString Zone = Args.Num() ? Args[0] : TEXT("right_forearm");
			TInlineComponentArray<UCapsuleComponent*> Capsules(A);
			for (UCapsuleComponent* C : Capsules)
			{
				if (C->GetName() == TEXT("Touch_") + Zone)
				{
					const FTransform At(C->GetComponentLocation() + FVector(0, 0, 30.0));
					APDProp* Ball = World->SpawnActorDeferred<APDProp>(APDProp::StaticClass(), At);
					Ball->Kind = EPDPropKind::Sphere;
					Ball->Color = FLinearColor(1.f, 0.85f, 0.1f);
					Ball->FinishSpawning(At);
					Ball->SetLifeSpan(3.f);
					return;
				}
			}
			UE_LOG(LogPartialDive, Warning, TEXT("touchtest: no touch zone '%s'"), *Zone);
		}));

	FAutoConsoleCommandWithWorldAndArgs InputCmd(TEXT("pdive.input"), TEXT("Bridge input source: intent | controller"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
			if (UPartialDiveBridgeSubsystem* B = GI ? GI->GetSubsystem<UPartialDiveBridgeSubsystem>() : nullptr)
			{
				B->SetInputSource(Args.Num() && Args[0].ToLower() == TEXT("controller") ? EPDInputSource::Controller : EPDInputSource::Intent);
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs AfterCmd(TEXT("pdive.after"), TEXT("pdive.after <seconds> <console command...>"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2)
			{
				return;
			}
			const float Delay = FCString::Atof(*Args[0]);
			const FString Command = FString::Join(TArrayView<const FString>(Args).RightChop(1), TEXT(" "));
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Command](float)
			{
				// Resolve the game world at execution time and go through the player controller, so
				// viewport commands (HighResShot, stat ...) work exactly as if typed in the console.
				for (const FWorldContext& Ctx : GEngine ? GEngine->GetWorldContexts() : TIndirectArray<FWorldContext>())
				{
					UWorld* W = Ctx.World();
					if (W && (Ctx.WorldType == EWorldType::Game || Ctx.WorldType == EWorldType::PIE))
					{
						if (APlayerController* PC = W->GetFirstPlayerController())
						{
							PC->ConsoleCommand(Command, /*bWriteToLog=*/true);
							return false;
						}
						GEngine->Exec(W, *Command);
						return false;
					}
				}
				return false; // once
			}), Delay);
		}));
}
