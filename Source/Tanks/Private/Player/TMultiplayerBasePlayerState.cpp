// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TMultiplayerBasePlayerState.h"

// Engine Includes
#include "Net/UnrealNetwork.h"



ATMultiplayerBasePlayerState::ATMultiplayerBasePlayerState()
{
	PlayerScoreStats.Score = 0;
	PlayerScoreStats.Deaths = 0;
	PlayerScoreStats.Kills = 0;
	PlayerScoreStats.Ping = 0;
}


void ATMultiplayerBasePlayerState::UpdatePlayerScoreStats(int32 DeltaScore, bool bAddKill, bool bAddDeath)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return;

	PlayerScoreStats.Score += DeltaScore;

	if (bAddKill)
	{
		++PlayerScoreStats.Kills;
	}

	if (bAddKill)
	{
		++PlayerScoreStats.Deaths;
	}

	OnRep_PlayerScoreStats();
}


void ATMultiplayerBasePlayerState::GetPlayerScoreStats(FPlayerScoreStats& PlayerStats) const
{
	PlayerStats.PlayerName = GetPlayerName();
	PlayerStats.Deaths = PlayerScoreStats.Deaths;
	PlayerStats.Kills = PlayerScoreStats.Kills;
	PlayerStats.Score = PlayerScoreStats.Score;
	PlayerStats.Ping = GetPing();
}

void ATMultiplayerBasePlayerState::OnRep_PlayerScoreStats()
{
	OnPlayerScoreChange.Broadcast(PlayerScoreStats.Score, PlayerScoreStats.Kills, PlayerScoreStats.Deaths);
}


void ATMultiplayerBasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATMultiplayerBasePlayerState, PlayerScoreStats);
}


