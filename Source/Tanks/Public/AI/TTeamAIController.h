// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "TTeamAIController.generated.h"

class UAIPerceptionComponent;

/**
 * 
 */
UCLASS()
class TANKS_API ATTeamAIController : public AAIController
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	UPROPERTY(EditAnywhere, Category = "TeamID")
	uint8 TeamNumber;

	UPROPERTY(VisibleAnywhere, Category = "Team")
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

public:

	ATTeamAIController();

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	FGenericTeamId GetGenericTeamId() const;

	void SetTeamNumber(uint8 Number);

	void GetTargetActors(TArray<AActor*>& TargetActors);
	
};
