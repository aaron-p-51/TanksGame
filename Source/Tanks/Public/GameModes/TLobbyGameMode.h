// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStates/TLobbyGameState.h"
#include "GameFramework/GameMode.h"
#include "TLobbyGameMode.generated.h"

/** Forward declarations */
class FNamedOnlineSession;


/**
 * GameMode for Multiplayer Lobby Map
 */
UCLASS()
class TANKS_API ATLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

/**
 * Members
 */
private:

	/** Timer to update date in LobbyGameState, used as slower rate tick */
	FTimerHandle TimerHandle_DefaultTick;

	/** Timer to manage loading the next map once start criteria has been meet */
	FTimerHandle TimerHandle_LoadNextMap;

	/**
	 * Timer duration for TimerHandle_LoadNextMap. After match starting criteria is meet delay for this amount of time.
	 * Allows player widgets to display count down
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	float WaitTimeAfterIsReadyToStart;

	/** Players currently in the session */
	UPROPERTY()
	int32 PlayerCount;


/**
 * Methods
 */

private:

	/** Init TimerHandle_DefaultTick */
	virtual void PreInitializeComponents() override;

	/** Used as slower rate tick to update lobby data */
	void DefaultTick();

	/**
	 * Update LobbyData when criteria has been met to move to the next map.
	 * 
	 * @parma LobbyData		- Out parameter will up updated with new data
	 */
	void ProcessLoadNextMap(FLobbyData& LobbyData);

	/**
	 * Update LobbyData with current session data. Will determine if criteria is meet to move to next map
	 * 
	 * @param LobbyData		- Out parameter will update with new data
	 */
	void UpdateLobbyData(FLobbyData& LobbyData);

	/** Get Session data */
	int32 GetSessionMaxPlayers(FNamedOnlineSession* NamedOnlineSession) const;
	FString GetSessionServerName(FNamedOnlineSession* NamedOnlineSession) const;

	/** Get the total amount of players who have voted to start */
	int32 GetTotalVotesToStart() const;

	/**
	 * Start criteria for match (load next map)
	 * 
	 * @ VotesToStart	- total amount of player who have voted to start the match
	 * @ MaxPlayers		- Max players in session
	 */
	bool IsReadyToStart(const int32 VotesToStart, const int32 MaxPlayers) const;

	/** Load the next map, uses ServerTravel */
	void LoadNextMap();


public:

	ATLobbyGameMode();

	/** Update  PlayerCount */
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

};
