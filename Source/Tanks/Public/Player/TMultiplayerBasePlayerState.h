// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TMultiplayerBasePlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerScoreChange, int32, Score, int32, Kills, int32, Deaths);

USTRUCT(BlueprintType)
struct FPlayerScoreStats
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int32 Score;

	UPROPERTY()
	int32 Kills;

	UPROPERTY()
	int32 Deaths;

	UPROPERTY()
	uint8 Ping;
};

/**
 * Base class for Multiplayer Player State
 */
UCLASS()
class TANKS_API ATMultiplayerBasePlayerState : public APlayerState
{
	GENERATED_BODY()

/**
 * Members
 */
public:

	UPROPERTY(BlueprintAssignable)
	FOnPlayerScoreChange OnPlayerScoreChange;
	

protected:

	/** Used to keep track for player data, used to display score during match */
	UPROPERTY(ReplicatedUsing=OnRep_PlayerScoreStats)
	FPlayerScoreStats PlayerScoreStats;

/**
 * Methods
 */

private:

	UFUNCTION()
	void OnRep_PlayerScoreStats();

public:

	 ATMultiplayerBasePlayerState();

	/**
	 * [Server] Update Score stats for player
	 * @param DeltaScore	Score Change
	 * @param bAddKill		Add a kill to this player's PlayerScoreStats
	 * @param bAddDeath		Add a death to this player's PlayerScoreStats
	 */
	void UpdatePlayerScoreStats(int32 DeltaScore, bool bAddKill, bool bAddDeath);

	/**
	 * [Client] Get The players current PlayerScoreStats
	 * @params PlayStats	out parameter for PlayerScoreStats
	 */
	void GetPlayerScoreStats(FPlayerScoreStats& PlayerStats) const;
};
