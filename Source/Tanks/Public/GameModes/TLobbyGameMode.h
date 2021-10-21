// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStates/TLobbyGameState.h"
#include "GameFramework/GameMode.h"
#include "TLobbyGameMode.generated.h"



/**
 * 
 */
UCLASS()
class TANKS_API ATLobbyGameMode : public AGameMode
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

	UPROPERTY()
	int32 PlayerCount;



/**
 * Methods
 */

private:

	/** Init TimerHandle_DefaultTick */
	virtual void PreInitializeComponents() override;

	int32 GetSessionMaxPlayers() const;
	FString GetSessionServerName() const;
	int32 GetSessionCurrentPlayers() const;

protected:

	/** Used as slower rate tick to update lobby data */
	void DefaultTick();

	/**
	 * Get the total amout of players who have voted to start
	 *
	 * return - number of players ready to start
	 */
	int32 GetTotalVotesToStart() const;

	/**
	 * Start criteria for match (load the map will the match will take place
	 * @ VotesToStart	- total amount of player who have voted to start the match
	 * @ MaxPlayers		- Max players in session
	 */
	bool IsReadyToStart(const int32 VotesToStart, const int32 MaxPlayers) const;

	/** Load the multiplayer map */
	void LoadNextMap();


public:

	ATLobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	
};
