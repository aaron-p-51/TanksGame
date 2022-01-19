// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/TAirItemSpawnerTransport.h"
#include "TItemDropAircraft.generated.h"


/** Forward declarations */
class USkeletalMeshComponent;
class UAudioComponent;


/**
 * Base for ATAirItemSpawner to transport items
 */
UCLASS()
class TANKS_API ATItemDropAircraft : public ATAirItemSpawnerTransport
{
	GENERATED_BODY()

/**
 * Members
 */

private:
	
	/** Mesh for Aircraft */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	/** Aircraft audio */
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	UAudioComponent* AudioComp;


 /**
  * Methods
  */
	
public:	
	// Sets default values for this actor's properties
	ATItemDropAircraft();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:

	/**
	 * [Server + Client] Called when this actor CurrentTravelDistance exceeds distance to transport destination set in TransportTravelData.DistanceToTransportDestination. See parent class
	 * Alert AirItemSpawner this actor reached its target location
	 */
	virtual void OnReachedTravelTransportDestination() override;

};
