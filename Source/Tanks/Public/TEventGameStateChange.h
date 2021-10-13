// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TMWEventBase.h"
#include "TTanksTypes.h"
#include "TEventGameStateChange.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TANKS_API UTEventGameStateChange : public UTMWEventBase
{
	GENERATED_BODY()

/**
 * members
 */

 public:

	UTEventGameStateChange();

	/** New State the GameMode has transitioned to  */
	EGameInProgressState NewGameState;

	/** Amount of seconds NewGameState will last */
	int32 StateTimeSeconds;

	/** Message for event recipients */
	FString StateMessage;
	
};
