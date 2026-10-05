#include "PDGameMode.h"

#include "PDAvatar.h"

APDGameMode::APDGameMode()
{
	DefaultPawnClass = APDAvatar::StaticClass();
}
