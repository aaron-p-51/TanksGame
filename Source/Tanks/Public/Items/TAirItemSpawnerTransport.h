// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAirItemSpawnerTransport.generated.h"

/** Forward declarations */
class ATItemDropZone;
class ATAirItemSpawner;

/** Data used to define movement and state changes */
USTRUCT(BlueprintType)
struct FTransportTravelData
{
	GENERATED_BODY()

	/** Target location in worldspace where this actor will move to */
	UPROPERTY()
	FVector TargetLocation;

	/** Distance from spawn point of derived actors when transport destination is reached */
	UPROPERTY()
	float DistanceToTransportDestination;

	/** Max distance this actor should travel. */
	UPROPERTY()
	float MaxTravelDistance;

	/** Is set to true when valid entries are set */
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bSpawnerDataSet = false;

	FTransportTravelData() {}

	FTransportTravelData(FVector Location, float TransportDestinationDistance, float MaxDistance) :
		TargetLocation(Location), DistanceToTransportDestination(TransportDestinationDistance), MaxTravelDistance(MaxDistance)
	{}

};


/**
 *	Base class for actors used by ATAirItemSpawner to transport items 
 */
UCLASS(Abstract, Blueprintable)
class TANKS_API ATAirItemSpawnerTransport : public AActor
{
	GENERATED_BODY()
	
/**
 * Members *
 */
protected:

	/**************************************************************************************/
	/*	Configuration
	/**************************************************************************************/

	/** Speed this actor moves across the level from its spawn point to TransportTravelData.TargetLocation */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float MovementSpeed;

	/** Define the movement and state change of this actor.  */
	UPROPERTY(VisibleAnywhere, Category = "Configuration")
	FTransportTravelData TransportTravelData;


	/**************************************************************************************/
	/*	State
	/**************************************************************************************/

	/** Set when CurrentTravelDistance exceeds DistanceToTransportDestination */
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bReachedTravelTransportDestination = false;

	/** Allow for movement to progress from spawn point to TargetLocation */
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bEnableMovement;

	/** Total travel distance this actor has moved in the level */
	UPROPERTY(VisibleAnywhere, Category = "State")
	float TotalTravelDistance;


	/**************************************************************************************/
	/*	Cached references
	/**************************************************************************************/

	/** Reference to assigned ATItemDropZone instance. */
	UPROPERTY(VisibleAnywhere)
	ATItemDropZone* ItemDropZone;

	/** Reference to level ATAirITemSpawner */
	UPROPERTY(VisibleAnywhere)
	ATAirItemSpawner* AirItemSpawner;


/**
 * Methods *
 */

public:	

	// Sets default values for this actor's properties
	ATAirItemSpawnerTransport();

	/**
	 * [Server] Set data to define movement of the actor from its spawn point to target location
	 * 
	 * @param Zone			ATItemDropZone this ATAirITemSpawnerTransport is assigned to deliver an item to
	 * @param Spawner		ATAirItemSpawner in the level
	 * @Data				Defines target location distances to cause state changes 
	 */
	void SetAirItemSpawnerData(ATItemDropZone* Zone, ATAirItemSpawner* Spawner, FTransportTravelData Data);

	/** Get references to class members */
	ATItemDropZone* GetAssignedItemDropZone() const { return ItemDropZone; }
	ATAirItemSpawner* GetAirItemSpawner() const { return AirItemSpawner; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** [Server + Client] Move this actor towards its target location set in TransportTravelData.TargetLocation */
	void MoveTowardsTargetLocation(float DeltaTime);

	/** [Server + Client] Called when this actor CurrentTravelDistance exceeds distance to transport destination set in TransportTravelData.DistanceToTransportDestination */
	virtual void OnReachedTravelTransportDestination() PURE_VIRTUAL(ATAirItemSpawnerTransport::OnReachedTravelTransportDestination, );


private:

	/** Set TransportTravelData for all clients. RPC is used here because entries in TransportTravelData should only every be set once. */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTargetLocation(FTransportTravelData Data);

};
