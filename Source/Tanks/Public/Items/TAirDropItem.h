// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAirDropItem.generated.h"


class UStaticMeshComponent;
class USphereComponent;


UCLASS()
class TANKS_API ATAirDropItem : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */

protected:

	/** Collision for AirDropItem */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	/** Rate for actor is start disappearing (shrink) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AirDropItem")
	float ScaleRateChange;

	/** Fall Speed */
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "AirDropItem")
	float FallSpeed;


public:

	/** Location to travel to */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "AirDropItem")
	FVector TargetLocation;

	/** Should actor start disappear, will destroy after scale < 0 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "AirDropItem")
	bool bShouldDisappear;

 /**
  * Methods
  */

public:	
	// Sets default values for this actor's properties
	ATAirDropItem();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
