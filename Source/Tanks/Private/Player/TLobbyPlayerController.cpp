// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TLobbyPlayerController.h"

// Engine Includes
#include "Net/UnrealNetwork.h"

// Game Includes
#include "Player/TLobbyPlayerState.h"


void ATLobbyPlayerController::VoteToStart(bool bStart)
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerVoteToStart(bStart);
	}
	else
	{
		ATLobbyPlayerState* LobbyPlayerState = GetPlayerState<ATLobbyPlayerState>();
		if (LobbyPlayerState)
		{
			LobbyPlayerState->SetVoteToStart(bStart);
		}
	}
}


void ATLobbyPlayerController::ServerVoteToStart_Implementation(bool bStart)
{
	VoteToStart(bStart);
}
