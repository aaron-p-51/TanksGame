// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TLobbyGameState.generated.h"

/** Multiplayer session settings. */
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
	uint8 VotesNeededToStart;

	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsMatchStarting;

	UPROPERTY(BlueprintReadOnly)
	float TimeTillMatchStart;
};


/**
 * GameState for Multiplayer Lobby
 */
UCLASS()
class TANKS_API ATLobbyGameState : public AGameState
{
	GENERATED_BODY()

/**
 * Members
 */
private:

	/** Multiplayer session settings data */
	UPROPERTY(Replicated)
	FLobbyData LobbyData;


 /**
  * Methods
  */

public:

	/** Set initial values for LobbyData */
	ATLobbyGameState();

	/** Get the currently set LobbyData */
	UFUNCTION(BlueprintCallable)
	FLobbyData GetLobbyData() const { return LobbyData; }

	/** [Server] Set LobbyData */
	void SetLobbyData(FLobbyData Data);
	
};
