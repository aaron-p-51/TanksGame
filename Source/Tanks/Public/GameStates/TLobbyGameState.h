// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TLobbyGameState.generated.h"

USTRUCT(BlueprintType)
struct FLobbyData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	uint8 MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	uint8 CurrentPlayers;

	UPROPERTY(BlueprintReadOnly)
	uint8 VotesToStart;

	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMatchStarting;

	UPROPERTY(BlueprintReadOnly)
	float TimeTillMatchStart;

	
};


/**
 * 
 */
UCLASS()
class TANKS_API ATLobbyGameState : public AGameState
{
	GENERATED_BODY()

/**
 * Members
 */
protected:

	UPROPERTY(Replicated, BlueprintReadOnly)
	FLobbyData LobbyData;


 /**
  * Methods
  */

public:

	ATLobbyGameState();

	UFUNCTION(BlueprintCallable)
	FLobbyData GetLobbyData() const { return LobbyData; }

	void SetLobbyData(FLobbyData Data);
	
	
};
