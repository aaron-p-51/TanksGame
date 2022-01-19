

#include "TTanksTypes.generated.h"
#pragma once

/**
 * States that take place while GameMode MatchState InPorgress State is active
 * Once MatchState transitions to InProgress EGameInProgress will transition through all states below.
 * Game mode See TMultiplayerBaseGameMode.h will set the time for each state and transition from 
 * one state to the next
 */
UENUM(BlueprintType)
enum class EGameInProgressState : uint8
{
	EGINP_WaitingForInProgress,
	EGINP_StartPlayerDelay,
	EGINP_GameStartCountDown,
	EGINP_GameInProgress,
	EGINP_GameExitCountDown,
	EGINP_GameTimeExpired,

	EGINP_StateNotSpecified
};


USTRUCT(BlueprintType)
struct FTanksGameInProgressState
{
	GENERATED_BODY()

	UPROPERTY()
	EGameInProgressState State;

	UPROPERTY()
	uint32 RemainingSeconds;

	FTanksGameInProgressState() {}
	FTanksGameInProgressState(EGameInProgressState ProgressState, uint32 Seconds) : State(ProgressState), RemainingSeconds(Seconds) {}
};


UENUM(BlueprintType)
enum class EShootableWeaponType : uint8
{
	ESWT_Instant		UMETA(DisplayName = "Instant"),
	ESWT_Projectile		UMETA(DisplayName = "Projectile"),

	ESWT_DefaultMax		UMETA(DisplayName = "Default")
};


//UENUM(BlueprintType)
//enum class EShootableWeaponAmmo : uint8
//{
//	ESWA_MachineGunRounds	UMETA(DisplayName = "MachineGunRounds"),
//	ESWA_Rockets			UMETA(DisplayName = "Rockets"),
//	ESWA_HeavyShells		UMETA(DisplayName = "HeavyShells")
//};


//UENUM(BlueprintType)
//enum class EInfantryWeaponType : uint8
//{
//	EIWT_MachineGun			UMETA(DisplayName = "MachineGun"),
//	EIWT_RocketLauncher		UMETA(DisplayName = "RocketLauncher"),
//	EIWT_None				UMETA(DisplayName = "None"),
//
//	EIWT_DefaultMax			UMETA(DisplayName = "Default")
//};


UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Health				UMETA(DisplayName = "Health"),
	EIT_MachineGunRounds	UMETA(DisplayName = "MachineGunRounds"),
	EIT_Rockets				UMETA(DisplayName = "Rockets"),
	EIT_HeavyShell			UMETA(DisplayName = "HeavyShell"),

	EIT_NotSpecified		UMETA(DisplayName = "NotSpecified")
};

