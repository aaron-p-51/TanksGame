// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates\TLobbyGameState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"


ATLobbyGameState::ATLobbyGameState()
{
	LobbyData.MaxPlayers = 0;
	LobbyData.CurrentPlayers = 0;
	LobbyData.VotesToStart = 0;
	LobbyData.VotesNeededToStart = 0;
	LobbyData.ServerName = "Server";
	LobbyData.bIsMatchStarting = false;
	LobbyData.TimeTillMatchStart = 0.f;
}


void ATLobbyGameState::SetLobbyData(FLobbyData Data)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetLobbyData() Authority"));

		//
		//	Shallow copy, LobbyData = Data, is not working properly maybe has to do with LobbyData being replicated
		//

		LobbyData.MaxPlayers = Data.MaxPlayers;
		LobbyData.CurrentPlayers = Data.CurrentPlayers;
		LobbyData.VotesToStart = Data.VotesToStart;
		LobbyData.VotesNeededToStart = Data.VotesNeededToStart;
		LobbyData.ServerName = Data.ServerName;
		LobbyData.bIsMatchStarting = Data.bIsMatchStarting;
		LobbyData.TimeTillMatchStart = Data.TimeTillMatchStart;
	}
}


void ATLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATLobbyGameState, LobbyData);
}