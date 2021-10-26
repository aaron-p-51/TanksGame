// Fill out your copyright notice in the Description page of Project Settings.


#include "Player\TLobbyPlayerState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"

void ATLobbyPlayerState::SetVoteToStart(bool Value)
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerSetVoteToStart(Value);
	}

	bVoteToStart = Value;
}


void ATLobbyPlayerState::ServerSetVoteToStart_Implementation(bool Value)
{
	SetVoteToStart(Value);
}


void ATLobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATLobbyPlayerState, bVoteToStart);
}
