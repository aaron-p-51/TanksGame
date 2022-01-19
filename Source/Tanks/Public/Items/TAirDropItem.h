// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/TAirItemSpawnerTransport.h"
#include "TAirDropItem.generated.h"


/**
 * Base for ATAirItemSpawner to transport items
 */
UCLASS()
class TANKS_API ATAirDropItem : public ATAirItemSpawnerTransport
{
	GENERATED_BODY()
	
/**
 * Members
 */

private:

	/** Rate for actor is start disappearing (shrink). */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float ScaleRateChange;


 /**
  * Methods
  */

public:	
	// Sets default values for this actor's properties
	ATAirDropItem();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	/**
	 * [Server + Client] When this actor reaches its travel destination, ie OnReachedTravelTransportDestination is called.
	 * Begin to shrink this actors scale. When scale reaches 0.1f, destroy actor.
	 */
	void ShrinkAndDestroyActor(float DeltaTime);

	/**
	 * [Server + Client] Called when this actor CurrentTravelDistance exceeds distance to transport destination set in TransportTravelData.DistanceToTransportDestination. See parent class
	 * Alert AirItemSpawner this actor reached its target location
	 */
	virtual void OnReachedTravelTransportDestination() override;
};
