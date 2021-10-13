// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "TPlayerControllerTeam.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATPlayerControllerTeam : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:


	ATPlayerControllerTeam();

	UPROPERTY(EditDefaultsOnly)
	FGenericTeamId TeamId;

	FGenericTeamId GetGenericTeamId() const;

	
};
