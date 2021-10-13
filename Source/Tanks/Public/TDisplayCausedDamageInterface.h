// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TDisplayCausedDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTDisplayCausedDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TANKS_API ITDisplayCausedDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void DisplayDamageCaused(AActor* DamagedActor, float Damage) = 0;
};
