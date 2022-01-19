// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/TMultiplayerBaseGameState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"


bool ATMultiplayerBaseGameState::SetNextGameInProgressState(FTanksGameInProgressState NewGameInProgressState)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && NewGameInProgressState.State > GameInProgressState.State)
	{
		GameInProgressState = NewGameInProgressState;
		BroadcastStateChange();
		return true;
	}

	return false;
}


void ATMultiplayerBaseGameState::BroadcastStateChange()
{
	int32 RemaininTime = GameInProgressState.RemainingSeconds;

	switch (GameInProgressState.State)
	{
		case EGameInProgressState::EGINP_StartPlayerDelay:
			OnStartPlayerDelay.Broadcast(RemaininTime);
			break;

		case EGameInProgressState::EGINP_GameStartCountDown:
			OnGameStartCountDown.Broadcast(RemaininTime);
			break;

		case EGameInProgressState::EGINP_GameInProgress:
			OnGameInProgress.Broadcast(RemaininTime);
			break;

		case EGameInProgressState::EGINP_GameExitCountDown:
			OnGameExitCountDown.Broadcast(RemaininTime);
			break;

		case EGameInProgressState::EGINP_GameTimeExpired:
			OnGameTimeExpired.Broadcast();
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown State in ATMultiplayerBaseGameState::BroadcastStateChange()"));
			break;
	}

	
}


void ATMultiplayerBaseGameState::DecrementGameInProgressStateTime()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		--GameInProgressState.RemainingSeconds;
	}
}


FString ATMultiplayerBaseGameState::GetGameInProgressStateName(EGameInProgressState State) const
{
	FString StringName = FString("Unknown");
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameInProgressState"), true);
	if (EnumPtr)
	{
		StringName = EnumPtr->GetDisplayNameTextByIndex(static_cast<uint8>(State)).ToString();
	}

	return StringName;
}


void ATMultiplayerBaseGameState::OnRepGameInProgressStateChange(FTanksGameInProgressState OldGameInProgressState)
{
	if (OldGameInProgressState.State != GameInProgressState.State)
	{
		BroadcastStateChange();
	}
}


void ATMultiplayerBaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATMultiplayerBaseGameState, GameInProgressState);
	
}


