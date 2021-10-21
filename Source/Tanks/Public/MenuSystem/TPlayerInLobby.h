// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPlayerInLobby.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TANKS_API UTPlayerInLobby : public UUserWidget
{
	GENERATED_BODY()


/**
 * Members
 */

protected:

	/**************************************************************************/
	/* Widgets */
	/**************************************************************************/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerNameText;

/**
 * Methods
 */

public:

	void SetPlayerName(FText PlayerName);

	FText GetPlayerName() const;


};
