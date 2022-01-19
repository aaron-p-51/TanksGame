// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TDeathmatchGameMode.h"


// Game Includes
#include "Player/TMultiplayerBasePlayerState.h"


ATDeathmatchGameMode::ATDeathmatchGameMode()
{
	PointsPerPlayerDamage = 1;
	PointsPerPlayerKill = 100;
}


float ATDeathmatchGameMode::AssignPlayerDamageScore(AController* InstigatorController, AController* VictimController, float DamageAmount, bool bVictimPawnDied)
{
	float DamageScore = 0.f;
	if (InstigatorController && VictimController && DamageAmount != 0.0f)
	{
		DamageScore = DamageAmount * PointsPerPlayerDamage;
		if (bVictimPawnDied)
		{
			DamageAmount += PointsPerPlayerKill;
		}

		// Update Instigator player Score and kills if victim pawn died
		if (DamageScore > 0.f)
		{
			ATMultiplayerBasePlayerState* InstigatorPlayerState = InstigatorController->GetPlayerState<ATMultiplayerBasePlayerState>();
			if (InstigatorPlayerState)
			{
				InstigatorPlayerState->UpdatePlayerScoreStats(DamageScore, bVictimPawnDied, false);
			}
		}

		// Update victim deaths if Victim pawn died
		if (bVictimPawnDied)
		{
			ATMultiplayerBasePlayerState* VictimPlayerState = VictimController->GetPlayerState<ATMultiplayerBasePlayerState>();
			if (VictimPlayerState)
			{
				VictimPlayerState->UpdatePlayerScoreStats(0.f, false, true);
			}
		}
	}

	return DamageScore;
}


void ATDeathmatchGameMode::RestartDeadPlayers()
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// Find player controllers without pawn. Restart player
		APlayerController* PlayerController = It->Get();
		if (PlayerController && PlayerController->GetPawn() == nullptr)
		{
			RestartPlayer(PlayerController);
		}
	}
}
