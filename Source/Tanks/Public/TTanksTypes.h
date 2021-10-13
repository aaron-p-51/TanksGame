

#include "TTanksTypes.generated.h"
#pragma once

/**
 * States that take place which take place while GameMode MatchState InPorgress State is active
 * Once MatchState transitions to InProgress EGameInProgress will transition to EGINP_WaitingForPlayersToSpawn
 * and will transition sequentially till the end of the match
 *
 */
UENUM(BlueprintType)
enum class EGameInProgressState : uint8
{
	EGINP_WaitingForInProgress,
	EGINP_StartPlayerDelay,
	EGINP_GameStartCountDown,
	EGINP_GameInProgress,
	EGINP_GameTimeExpired,

	EGINP_StateNotSpecified
};

UENUM(BlueprintType)
enum class EShootableWeaponType : uint8
{
	ESWT_Instant		UMETA(DisplayName = "Instant"),
	ESWT_Projectile		UMETA(DisplayName = "Projectile"),

	ESWT_DefaultMax		UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EInfantryWeaponType : uint8
{
	EIWT_MachineGun			UMETA(DisplayName = "MachineGun"),
	EIWT_RocketLauncher		UMETA(DisplayName = "RocketLauncher"),
	EIWT_None				UMETA(DisplayName = "None"),

	EIWT_DefaultMax			UMETA(DisplayName = "Default")
};

