// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "GameFramework/GameMode.h"
#include "TMultiplayerBaseGameMode.generated.h"


/**
 * Base class for all multiplayer game modes
 */
UCLASS()
class TANKS_API ATMultiplayerBaseGameMode : public AGameMode
{
	GENERATED_BODY()

/**
 * Members
 */


private:


	/**************************************************************************/
	/* Game In Progress State Times. 
	 *	See TTanksTypes.h for EGameInProgressState definition */
	/**************************************************************************/

	/** 
	 * Amount of time in seconds for StartPlayerDelay state. Set to 0 if StartPlayerDelay should not be used.
	 * StartPlayerDelay state will start when GameMode transitions to InProgress and HandleMatchHasStarted() is called
	 */
	UPROPERTY(EditAnywhere, Category = "GameInProgressStates")
	uint32 StartPlayersDelaySeconds;

	/**
	 * Amount of time in seconds for GameStartCountDown state. Set to 0 if GameStartCountDown should not be used
	 * GameStartCountDown state is for counting down game match start time.
	 */
	UPROPERTY(EditAnywhere, Category = "GameInProgressStates")
	uint32 GameStartCountDownSeconds;

	/**
	 * Amount of time in seconds for GameInProgress state. Set to 0 if GameInProgress should not be used
	 * GameInProgress state is when the match is actually played. 
	 */
	UPROPERTY(EditAnywhere, Category = "GameInProgressStates")
	uint32 GameInProgressSeconds;

	/**
	 * Amount of time in seconds for GameExitCountDown state. Set to 0 if GameExitCountDown should not be used.
	 * GameExitCountDown state is after GameInProgress state ends and before traveling back to Multiplayer lobby
	 */
	UPROPERTY(EditAnywhere, Category = "GameInProgressStates")
	int32 GameExitCountDownSeconds;

	/** TimerHandle to control match flow, will act as tick, called once per second */
	FTimerHandle TimerHandle_DefaultTimer;


	/**
	 * GameInProgressStates used in this game mode. Will be all states that have a time greater than 0 above.
	 */
	TArray<FTanksGameInProgressState> InProgressStates;

	/** Current state in InProgressStates  */
	int32 InProgressStatesIndex = 0;


/**
 * Methods
 */

 public:

	 ATMultiplayerBaseGameMode();

	 /**
	  * Assign score to player for damaging another player. This method should be overridden in derived classes for different types of game modes.
	  * 
	  * @param InstigatorController		Controller of pawn who caused damage to pawn controlled by VictimController
	  * @param VictimController			Controller of pawn who received damage from InstigatorController controlled pawn
	  * @param DamageAmount				Amount of damage InstigatorController caused
	  * @param bVictimPawnDied			VictimController's pawn died as a result of DamageAmount
	  * 
	  * return							Score result from damage
	  */
	 virtual float AssignPlayerDamageScore(AController* InstigatorController, AController* VictimController, float DamageAmount, bool bVictimPawnDied) { return 0.f; }


protected:

	/** Tick once a seconds to control progression through GameInProgressStates */
	virtual void DefaultTimerTick();

	/** Called when MatchState transitions from WaitingToStart to InProgress  */
	virtual void HandleMatchHasStarted() override;

	/**
	 * Restart (respawn) player pawn's that have died. Called from DefaultTimerTick() will be called once per second. Derived classes should
	 * override to implement how to restart players for particular game mode
	 */
	virtual void RestartDeadPlayers() {}

	

private:

	/** Called right before components are initialized, only called during gameplay */
	virtual void PreInitializeComponents() override;

	/** Change to the next state. Will progress through all states in InProgressStates */
	bool TryChangeToNextGameInProgressState();

	/** Server travel back to MultiplayerLobby Map */
	void TravelToLobby();

	/** Set all states used in InProgressStates. Will always assing WaitingForInProgress as first state and EGINP_GameTimeExpired as last state */
	void SetInProgressStates();
};
