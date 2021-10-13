// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TMultiplayerScoreBoard.generated.h"

class UVerticalBox;
class UTPlayerScoreWidget;


/**
 * 
 */
UCLASS()
class TANKS_API UTMultiplayerScoreBoard : public UUserWidget
{
	GENERATED_BODY()

/**
 * Members
 */
protected:


	/** Box where Team A's Players are listed */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UVerticalBox* TeamAPlayersBox;

	/** Update Score */
	FTimerHandle TimerHandle_UpdateScore;

public:

	bool bCanToggleShowBoard = true;


 /**
  * Methods
  */

 protected:

	/** Update Score for players will get scores from BasePlayerState  */
	void UpdateScore();

 public:



	/** Show Scoreboad, will continue to update until called with bShow == false  */
	void ShowScoreBoard(bool bShow);

	UFUNCTION(BlueprintImplementableEvent)
	void SetScoreBoardTitleText(const FString& Value);
	
};
