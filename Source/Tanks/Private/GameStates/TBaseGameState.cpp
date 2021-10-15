// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/TBaseGameState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"


void ATBaseGameState::TravelToLobby()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel("/Game/Tanks/Maps/MultiplayerLobby?listen");
		}
	}
}
	

void ATBaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATBaseGameState, RemainingTime);
}