// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TBaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATBaseGameState : public AGameState
{
	GENERATED_BODY()

/*
* Members
*/

public:

	/** Amount of seconds left for the current GameInProgressState, defined in GameMode  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 RemainingTime = 0;


/*
* Methods
*/

public:

	/** [Server] Travel back to MultiplayerLobby */
	void TravelToLobby();
	
};
