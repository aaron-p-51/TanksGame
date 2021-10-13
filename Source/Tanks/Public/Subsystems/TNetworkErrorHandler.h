// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TNetworkErrorHandler.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API UTNetworkErrorHandler : public UGameInstanceSubsystem
{
	GENERATED_BODY()


/**
 * Members
 */



 /**
  * Methods
  */

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:

	/** Function bound to OnNetworkFailure, will push errors to ErrorQueue when called */
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);


	
};
