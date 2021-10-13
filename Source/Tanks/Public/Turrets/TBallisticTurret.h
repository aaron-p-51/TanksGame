// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turrets/TTurretBase.h"
#include "TBallisticTurret.generated.h"

class UTAIFireProjectile;

/**
 * 
 */
UCLASS()
class TANKS_API ATBallisticTurret : public ATTurretBase
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProjectile")
	UTAIFireProjectile* FireProjectileComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	float TimeBetweenFires;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	float TimeBetweenFiresDeviation;

	FTimerHandle TimerHandle_FireTurret;

 /**
  * Methods
  */

protected:

	virtual void BeginPlay() override;

	void FireTurret();

	float GetNextFireTime();

public:

	ATBallisticTurret();
	
};
