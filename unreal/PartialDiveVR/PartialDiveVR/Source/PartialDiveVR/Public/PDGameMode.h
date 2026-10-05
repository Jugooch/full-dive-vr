#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PDGameMode.generated.h"

/** Lab game mode: spawns the full-body APDAvatar. Set as the GameMode Override of experiment levels. */
UCLASS()
class PARTIALDIVEVR_API APDGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APDGameMode();
};
