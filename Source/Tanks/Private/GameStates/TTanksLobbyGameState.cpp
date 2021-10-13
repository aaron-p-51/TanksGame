// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/TTanksLobbyGameState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"


ATTanksLobbyGameState::ATTanksLobbyGameState()
{

	MultiplayerLobbyData.bIsStarting = false;
	MultiplayerLobbyData.TimeTillStartDuration = 5;

	SetReplicates(true);
}


void ATTanksLobbyGameState::SetMultiplayerLobbyData(FMultiplayerLobbyData Data)
{
	MultiplayerLobbyData = Data;
}


void ATTanksLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTanksLobbyGameState, MultiplayerLobbyData);

}
