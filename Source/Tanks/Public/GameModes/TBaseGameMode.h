// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TEventGameStateChange.h"
#include "TTanksTypes.h"
#include "GameFramework/GameMode.h"
#include "TBaseGameMode.generated.h"

class APlayerController;
class ATSpawnLocation;
class APlayerStart;





/**
 * 
 */
UCLASS()
class TANKS_API ATBaseGameMode : public AGameMode
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	/** Current State of EGameInProgressState, which all happen in MatchState InProgress State*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EGameInProgressState GameInProgressState;

	/** Enable Start Player Delay, this is amount of time to wait for all players to join. If there are no more players
	 * traveling from last map then delay will be be canceled*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|StartPlayerDelay")
	bool bUseStartPlayersDelay;

	/** If all players have finished traveling to the map then automatically go to the next state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|StartPlayerDelay", meta = (EditCondition = "bStartPlayersDelay"))
	bool bNextStateIfAllPlayers;

	/** Amount of time in seconds to wait to init players (setup GUIs) after MatchState transitions to InProgress see bStartPlayersDelay   */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|StartPlayerDelay", meta = (EditCondition = "bStartPlayersDelay"))
	int32 StartPlayersDelaySeconds;

	/** Message to send to Clients when EGINP_StartPlayerDelay becomes active state, update message on GUI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|StartPlayerDelay", meta = (EditCondition = "bStartPlayersDelay"))
	FString StartPlayerDelayMessage;

	/** Used to display could down and allow players to get ready for match/game start */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameStartCountDown")
	bool bUseGameStartCountDown;

	/** Amount of time in seconds for count down time for players to display  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameStartCountDown", meta = (EditCondition = "bUseMatchStartCountDown"))
	int32 GameStartCountDownSeconds;

	/** Message to send to Clients when EGINP_GameStartCountDown becomes active state, update message on GUI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameStartCountDown", meta = (EditCondition = "bUseMatchStartCountDown"))
	FString GameStartCountDownMessage;

	/**
	 * Use a time limit for game play, if set to false GameTimeSeconds will be set to -1 indicating there is not game time
	 * EGINP_GameInProgress will still be active state when states preceding it finishes
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameTime")
	bool bUseGameTime;

	/** Total match time the players have to play the match. Starts after MatchStartCountDownTime expires */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameTime", meta = (EditCondition = "bUseGameTime"))
	int32 GameTimeSeconds;

	/** Message to send to Clients when EGINP_GameInProgress becomes active state, update message on GUI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameTime")
	FString GameTimeMessage;

	/** Used to display messages to clients after match/game finishes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameTimeToExit")
	bool bUseGameTimeToExit;

	/** Amount of time is seconds to wait after MatchTimeSeconds to travel back to MultiplayerLobby  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameTimeToExit", meta = (EditCondition = "bUseGameTimeToExit"))
	int32 GameTimeToExitSeconds;

	/** Message to send to Clients when EGINP_GameTimeExpired becomes active state, update message on GUI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameStates|GameTimeToExit", meta = (EditCondition = "bUseGameTimeToExit"))
	FString GameTimeToExitMessage;

	/** Amount of points to assign Instigator player when damaging another player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PointsPerPlayerDamage;

	/** Amount of points to assign Instigator player when killing another player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PointsPerPlayerKill;

	/* Timer handle to star players, time will be set to StartPlayersDelayTime TODO: Check if can be removed by creating TimerHandle locally*/
	FTimerHandle TimerHandle_StartPlayersDelay;

	/* Timer handle to start match, TODO: Check if can be removed by creating TimerHandle */
	FTimerHandle TimerHandle_StartMatchTimeRemainingCounter;

	/** Delay to change from one EGameInProgressState to another after conditions are met */
	FTimerHandle TimerHandle_DelayChangeMatchState;

	/** TimerHandle to control match flow, will act as tick, called EffectiveTimeDialation rate, (once per second) */
	FTimerHandle TimerHandle_DefaultTimer;

	
	UTEventGameStateChange* GameMyStateChange;

	//float MatchTimeOffset;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APlayerStart> SpawnLocationClass;*/

	/*UPROPERTY(VisibleAnywhere)
	TArray<APlayerStart*> SpawnLocations;*/

 /**
  * Methods
  */

private:

	/** Called right before components are initialized, only called during gameplay */
	virtual void PreInitializeComponents() override;

protected:

	///** Respon pawns for all dead players */
	//virtual void RespawnDeadPlayers();

	/** Called when MatchState transitions from WaitingToStart to InProgress  */
	virtual void HandleMatchHasStarted() override;

	///** Signal to all players the match is about to start */
	//virtual void StartPlayers();

	///**
	// * Check to see if spawn point is okay to spawn player, based on distance between SpawnPoint and other Players
	// * @parma SpawnPoint - Spawn point to check
	// * @param Player - Player to spawn
	// */
	//bool IsSpawnPointPerferred(APlayerStart* SpawnPoint, AController* Player) const;

	///**
	// * Choose a spawn point for players when spawning into map. Player will be matched with the index in GameState PlayerArray
	// * with the PlayerStart PlayerStartTags
	// * @param Player - Player to spawn
	// */
	//APlayerStart* ChoosePlayerStartPreMatch(AController* Player);

	///**
	// * Choose a spawn point for player when respawning (after death) while the match is in EGINP_MatchInProgress state
	// * @param Player - Player to respawn
	// */
	//APlayerStart* ChoosePlayerStartDuringMatch(AController* Player);

	///**
	// * Change EGameInProgressState state, states must go in order they are defined. EGINP_WaitingForInProgress,
	// * EGINP_WaitingForPlayersToSpawn, EGINP_MatchInProgress, EGINP_MatchTimeExpired
	// * 
	// * @param NewState - NewState to transition to
	// */
	//virtual void ChangeGameInProgressState(EGameInProgressState NewState);

	/** Control match flow, will act as tick based on EffectiveTimeDilation (normally once per second) */
	virtual void DefaultTimer();

	/** Travel back to MultiplayerLobby */
	void TravelBackToLobby();

	void ChangeToGameInProgress();

	void ChangeToGameTimeExpired();

	void ChangeToStartPlayerDelay();

	void ChangeToGameStartCountDown();


	virtual void CheckIfReadyForGameInProgressState();

	
	void ResetGameStateTimer(int32 Seconds);

	void SendGameStateChangeEvent(EGameInProgressState NewState, int32 StateDurationSeconds, const FString& StateMessage);


public:

	/** Default constructor */
	ATBaseGameMode();

	/**
	 * Return true if damage can be applied in the current match state (GameInProgressState
	 */
	virtual bool DamageValidInCurrentGameInProgressState();

	/**
	 * Assign points to player who instigated damage, other match types should override this.
	 * @param InstigatorPlayer - Player who caused damage
	 * @param DamagedActor - Actor who was damaged, may not be another player
	 * @param DamageAmount - Amount of damage caused
	 * @param bActorDied - Did DamagedActor die as a result of the damage
	 */
	virtual float AssignPlayerDamageScore(AController* InstigatorController, AController* VictimController, float DamageAmount, bool bVictimPawnDied);

	///**
	// * Currently not doing anything
	// */
	//virtual void PostLogin(APlayerController* NewPlayer) override;

	///**
	//* Return the 'best' player start for this player to spawn from
	//* Default implementation looks for a random unoccupied spot
	//*
	//* @param Player is the controller for whom we are choosing a playerstart
	//* @returns AActor chosen as player start (usually a PlayerStart)
	//*/
	//virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	///**
	//*Return the specific player start actor that should be used for the next spawn
	//* This will either use a previously saved startactor, or calls ChoosePlayerStart
	//*
	//*@param Player The AController for whom we are choosing a Player Start
	//* @param IncomingName Specifies the tag of a Player Start to use
	//* @returns Actor chosen as player start(usually a PlayerStart)
	//*/
	//virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;

};
