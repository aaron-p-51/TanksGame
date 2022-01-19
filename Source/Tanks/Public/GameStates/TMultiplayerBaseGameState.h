// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "GameFramework/GameState.h"
#include "TMultiplayerBaseGameState.generated.h"


/** Declare delegates for FTanksGameInProgressState change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartPlayerDelay, int32, StateTimeSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStartCountDown, int32, StateTimeSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameInProgress, int32, StateTimeSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameExitCountDown, int32, StateTimeSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameTimeExpired);


/**
 * Base class for all multiplayer game states
 */
UCLASS()
class TANKS_API ATMultiplayerBaseGameState : public AGameState
{
	GENERATED_BODY()

/**
 * Members
 */

 public:

	/** Events for FTanksGameInProgressState change. Called on Server and Clients  */
	FOnStartPlayerDelay OnStartPlayerDelay;
	FOnGameStartCountDown OnGameStartCountDown;
	FOnGameInProgress OnGameInProgress;
	FOnGameExitCountDown OnGameExitCountDown;
	FOnGameTimeExpired OnGameTimeExpired;


private:

	/** Current TanksGameInProgressState state is set from server and replicated to clients. See TTanksTypes.h for FTanksGameInProgressState definition  */
	UPROPERTY(ReplicatedUsing=OnRepGameInProgressStateChange, VisibleAnywhere)
	FTanksGameInProgressState GameInProgressState;


/**
 * Methods
 */

private:

	/** [Client] GameInProgressState changed and replicated to clients. Broadcasts new state change */
	UFUNCTION()
	void OnRepGameInProgressStateChange(FTanksGameInProgressState OldGameInProgressState);

	/** [Server + Client] Broadcast when new GameInProgressState is set. Depending on state appropriate Multicast delegate will be called */
	UFUNCTION()
	void BroadcastStateChange();

	/** [Server + Client] For Debug, get the ASCII name of GameInProgressState state */
	FString GetGameInProgressStateName(EGameInProgressState State) const;


public:

	/**
	 * [Server] Set the next game in progress state. The next state must follow after the current GameInProgressState. It does not
	 * need to be sequential, but must come after current GameInProgressState. ie, if GameInProgressState.State == EGINP_GameInProgress
	 * NewGameInProgressState.State must be EGINP_GameExitCountDown or greater. See TTanksTypes.h for EGameInProgressState definition 
	 */
	UFUNCTION()
	bool SetNextGameInProgressState(FTanksGameInProgressState NewGameInProgressState);

	/** [Server + Client] Get current GameInProgressState. GameInProgressState is replicated to all clients  */
	FTanksGameInProgressState GetGameInProgressState() const { return GameInProgressState; }

	/** [Server] Decrease the time remaining in current GameInProgressState */
	void DecrementGameInProgressStateTime();

};
