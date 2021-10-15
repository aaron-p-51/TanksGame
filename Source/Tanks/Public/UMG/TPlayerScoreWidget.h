// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPlayerScoreWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TANKS_API UTPlayerScoreWidget : public UUserWidget
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PingText;

 /**
  * Methods
  */

public:

	void SetPlayerScoreText(FText PlayerName, int32 Score, int32 Kills, int32 Deaths, int32 Ping);
	
};
