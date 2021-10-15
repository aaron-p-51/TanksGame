// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/TBaseAIController.h"
#include "TSoldierAIController.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATSoldierAIController : public ATBaseAIController
{
	GENERATED_BODY()

/**
 * Members
 */

 /**
  * Methods
  */

  public:

	virtual void FireAtTarget(float Duration, float DurationVariance) override;
	
};
