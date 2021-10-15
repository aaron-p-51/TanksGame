// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/TPlayerScoreWidget.h"

#include "Components/TextBlock.h"

void UTPlayerScoreWidget::SetPlayerScoreText(FText PlayerName, int32 Score, int32 Kills, int32 Deaths, int32 Ping)
{
	if (PlayerText)
	{
		PlayerText->SetText(PlayerName);
	}

	if (ScoreText)
	{
		ScoreText->SetText(FText::AsNumber(Score));
	}

	if (KillsText)
	{
		KillsText->SetText(FText::AsNumber(Kills));
	}

	if (DeathsText)
	{
		DeathsText->SetText(FText::AsNumber(Deaths));
	}

	if (PingText)
	{
		PingText->SetText(FText::AsNumber(Ping));
	}
}
