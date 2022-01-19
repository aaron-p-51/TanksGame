// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TTanksGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class TANKS_API UTTanksGameInstance : public UGameInstance
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer Settings")
	int32 MinMultiplayerSessionPlayerCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer Settings")
	int32 MaxMultiplaySessionPlayerCount = 8;

	
/**
 * Methods
 */

public:

	UTTanksGameInstance(const FObjectInitializer& ObjectInitializer);

	int32 GetMinMultiplayerSessionPlayerCount() const { return MinMultiplayerSessionPlayerCount; }
	int32 GetMaxMultiplaySessionPlayerCount() const { return MaxMultiplaySessionPlayerCount; }

};
