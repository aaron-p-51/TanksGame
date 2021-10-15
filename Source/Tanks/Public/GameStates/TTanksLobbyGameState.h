// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TTanksLobbyGameState.generated.h"


USTRUCT(BlueprintType)
struct FMultiplayerLobbyData
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 VotesToStart;

	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsStarting;

	/** Will not change, this is the total time to wait after the start criteria in GameMode has been met till
	* the next map will load */
	UPROPERTY(BlueprintReadOnly)
	float TimeTillStartDuration;

};

/**
 * 
 */
UCLASS()
class TANKS_API ATTanksLobbyGameState : public AGameState
{
	GENERATED_BODY()
	
/**
 *	Members * 
 */
protected:

	/** Data used for Multiplayer Lobby, Data is used primarily for Player Widgets */
	UPROPERTY(BlueprintReadOnly, Replicated)
	FMultiplayerLobbyData MultiplayerLobbyData;

 /**
  * Methods
  */

public:
	ATTanksLobbyGameState();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FMultiplayerLobbyData GetMultiplayerLobbyData() {return MultiplayerLobbyData;}

	void SetMultiplayerLobbyData(FMultiplayerLobbyData Data);

};
