// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TShootableWeaponOwner.generated.h"

// This class does not need to be modified.
//UINTERFACE(MinimalAPI, meta = (Blueprintable))
UINTERFACE(BlueprintType)
class UTShootableWeaponOwner : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TANKS_API ITShootableWeaponOwner
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void GetShootableWeaponFirePoint(FVector& FireLocation, FRotator& FireRotation) = 0;

	UFUNCTION()
	virtual class USceneComponent* GetAttachmentSceneComponent() = 0;

	UFUNCTION()
	virtual FName GetAttachmentSocketName() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartFire();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopFire();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsFiring();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Reload();

	
};
