// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/TMultiplayerScoreBoard.h"

#include "GameFramework/GameState.h"
#include "TimerManager.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

#include "Player/TBasePlayerState.h"
#include "UMG/TPlayerScoreWidget.h"



void UTMultiplayerScoreBoard::ShowScoreBoard(bool bShow)
{
	if (bCanToggleShowBoard)
	{
		if (bShow)
		{	
			this->AddToViewport();
			UpdateScore();
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_UpdateScore);
			this->RemoveFromViewport();
		}
	}
}


void UTMultiplayerScoreBoard::UpdateScore()
{
	// Update Score
	auto GS = GetWorld()->GetGameState<AGameState>();
	if (GS)
	{
		TArray<FPlayerScoreStats> PlayerScoreStats;
		auto PlayerStates = GS->PlayerArray;
		for (auto PlayerState : PlayerStates)
		{
			auto BasePlayerState = Cast<ATBasePlayerState>(PlayerState);
			if (BasePlayerState)
			{
				FPlayerScoreStats Stats;
				BasePlayerState->GetPlayerScoreStats(Stats);
				PlayerScoreStats.Add(Stats);
			}
		}

		// Sort Scores, highest score first and place in TeamPlayerBox
		PlayerScoreStats.Sort([](const FPlayerScoreStats& a, const FPlayerScoreStats& b) {return a.Score > b.Score; });
		if (TeamAPlayersBox)
		{
			for (int32 i = 0; i < TeamAPlayersBox->GetChildrenCount(); ++i)
			{
				UWidget* Child = TeamAPlayersBox->GetChildAt(i);
				auto ScoreWidget = Cast<UTPlayerScoreWidget>(Child);
				if (ScoreWidget)
				{
					if (i < PlayerScoreStats.Num())
					{
						ScoreWidget->SetVisibility(ESlateVisibility::Visible);
						ScoreWidget->SetPlayerScoreText(FText::FromString(PlayerScoreStats[i].PlayerName), PlayerScoreStats[i].Score, PlayerScoreStats[i].Kills, PlayerScoreStats[i].Deaths, PlayerScoreStats[i].Ping);
					}
					else
					{
						ScoreWidget->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateScore, this, &UTMultiplayerScoreBoard::UpdateScore, 1, true);
}



