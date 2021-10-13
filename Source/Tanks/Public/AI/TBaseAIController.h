// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "TBaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATBaseAIController : public AAIController
{
	GENERATED_BODY()

/**
 * Members
 */

public:
	
	/** Combat Target for this AI agent, set be BT */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* CurrentCombatTarget;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TeamID")
	uint8 TeamNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TeamID")
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	FTimerHandle TimerHandle_FireTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool bUseMaxAngleToFireAtTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat", meta = (EditCondition = "bUseMaxAngleToFireAtTarget"))
	float MaxAngleToFireAtTarget;


/**
 * Methods
 */
protected:

	bool WithinMaxAngleToTarget();
	
	
public:

	ATBaseAIController(const FObjectInitializer& ObjectInitializer);

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	/** Get this player GenericTeamID */
	UFUNCTION(BlueprintPure)
	FORCEINLINE FGenericTeamId GetGenericTeamId() const { return TeamID; } 

	UFUNCTION(BlueprintCallable)
	virtual void FireAtTarget(float Duration, float DurationVariance);

	UFUNCTION(BlueprintCallable)
	virtual void StopFireAtTarget();

};
