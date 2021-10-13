// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TTanksLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ATTanksLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

/**
 * Members 
 */
protected:

	/** Timer to update data, used as slower rate tick */
	FTimerHandle TimerHandle_DefaultTick;

	/** Timer to manage loading the next map once start criteria has been meet */
	FTimerHandle TimerHandle_LoadNextMap;

	/** Has TimerHandle_LoadNextMap been set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bNextMapLoadTimerSet;

	/** Timer duration for TimerHandle_LoadNextMap */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lobby")
	float WaitTimeAfterIsReadyToStart;

 /**
  * Methods
  */

 private:

	/** Init TimerHandle_DefaultTick */
	virtual void PreInitializeComponents() override;

protected:

	/** Used as slower rate tick to update lobby data */
	void DefaultTick();

	/**
	 * Get the total amout of players who have voted to start
	 * 
	 * return - number of players ready to start
	 */
	int32 GetTotalVotesToStart();

	/**
	 * Start criteria for match (load the map will the match will take place
	 * @ VotesToStart	- total amount of player who have voted to start the match
	 * @ MaxPlayers		- Max players in session
	 */
	bool IsReadyToStart(int32 VotesToStart, int32 MaxPlayers);

	/** Load the multiplayer map */
	void LoadNextMap();


public:

	ATTanksLobbyGameMode();

};
