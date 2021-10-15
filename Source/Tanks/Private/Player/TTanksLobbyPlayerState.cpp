// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TTanksLobbyPlayerState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"


ATTanksLobbyPlayerState::ATTanksLobbyPlayerState()
{
	bVoteToStart = false;
}


void ATTanksLobbyPlayerState::SetVoteToStart(bool bValue)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		bVoteToStart = bValue;
	}
}


void ATTanksLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTanksLobbyPlayerState, bVoteToStart);
}
