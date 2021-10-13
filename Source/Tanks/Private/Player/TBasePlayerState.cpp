// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TBasePlayerState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"


ATBasePlayerState::ATBasePlayerState()
{
	PlayerScoreStats.Score = 0;
	PlayerScoreStats.Deaths = 0;
	PlayerScoreStats.Kills = 0;
}


void ATBasePlayerState::UpdatePlayerScoreStats(int32 DeltaScore, bool bAddKill, bool bAddDeath)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		PlayerScoreStats.Score += DeltaScore;

		if (bAddKill)
		{
			++PlayerScoreStats.Kills;
		}

		if (bAddDeath)
		{
			++PlayerScoreStats.Deaths;
		}
	}
}


void ATBasePlayerState::GetPlayerScoreStats(FPlayerScoreStats& PlayerStats)
{
	PlayerStats.PlayerName = GetPlayerName();
	PlayerStats.Deaths = PlayerScoreStats.Deaths;
	PlayerStats.Kills = PlayerScoreStats.Kills;
	PlayerStats.Score = PlayerScoreStats.Score;
	PlayerStats.Ping = GetPing();
}


void ATBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATBasePlayerState, PlayerScoreStats);
}
