// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/TMultiplayerBaseGameMode.h"
#include "TDeathmatchGameMode.generated.h"

/**
 * Base class for Deathmatch GameMode
 */
UCLASS()
class TANKS_API ATDeathmatchGameMode : public ATMultiplayerBaseGameMode
{
	GENERATED_BODY()

/**
 * Members
 */

private:

	/**************************************************************************/
	/* Gameplay */
	/**************************************************************************/

	/** Points awarded to damaging player for each damage point caused to another player */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float PointsPerPlayerDamage;

	/** Points awarded to damaging player when damage applied killed another player */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float PointsPerPlayerKill;

/**
 * Methods
 */

 public:

	ATDeathmatchGameMode();

	/**
	 * Assign score to player for damaging another player. Using Gameplay members above calculate and and assign score to Instigator and Victim players
	 *
	 * @param InstigatorController		Controller of pawn who caused damage to pawn controlled by VictimController
	 * @param VictimController			Controller of pawn who received damage from InstigatorController controlled pawn
	 * @param DamageAmount				Amount of damage InstigatorController caused
	 * @param bVictimPawnDied			VictimController's pawn died as a result of DamageAmount
	 *
	 * return							Score result from damage
	 */
	virtual float AssignPlayerDamageScore(AController* InstigatorController, AController* VictimController, float DamageAmount, bool bVictimPawnDied) override;


protected:

	/** Restart players at random open player start location in map */
	virtual void RestartDeadPlayers() override;
	
};
