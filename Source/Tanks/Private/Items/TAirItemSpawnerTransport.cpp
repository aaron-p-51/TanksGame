// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TAirItemSpawnerTransport.h"


// Engine Includes
#include "Net/UnrealNetwork.h"


// Game Includes
#include "Items/TAirItemSpawner.h"
#include "Items/TItemDropZone.h"


// Sets default values
ATAirItemSpawnerTransport::ATAirItemSpawnerTransport()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReachedTravelTransportDestination = false;
	bEnableMovement = true;
	TotalTravelDistance = 0.f;

	SetReplicates(true);
}


void ATAirItemSpawnerTransport::SetAirItemSpawnerData(ATItemDropZone* Zone, ATAirItemSpawner* Spawner, FTransportTravelData Data)
{
	ItemDropZone = Zone;
	AirItemSpawner = Spawner;
	TransportTravelData = Data;
	TransportTravelData.bSpawnerDataSet = true;
}


// Called when the game starts or when spawned
void ATAirItemSpawnerTransport::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{		
		// Clients will not move or check for distances before this RPC is processed
		MulticastSetTargetLocation(TransportTravelData);
	}
}


// Called every frame
void ATAirItemSpawnerTransport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TransportTravelData.bSpawnerDataSet)
	{
		if (!bReachedTravelTransportDestination && TotalTravelDistance > TransportTravelData.DistanceToTransportDestination)
		{
			bReachedTravelTransportDestination = true;
			OnReachedTravelTransportDestination();
		}

		if (bEnableMovement)
		{
			MoveTowardsTargetLocation(DeltaTime);
		}
	}
}


void ATAirItemSpawnerTransport::MoveTowardsTargetLocation(float DeltaTime)
{
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TransportTravelData.TargetLocation, DeltaTime, MovementSpeed));
	TotalTravelDistance += DeltaTime * MovementSpeed;
}


void ATAirItemSpawnerTransport::MulticastSetTargetLocation_Implementation(FTransportTravelData Data)/*const FVector& Location, float DistanceToTrave, float MaxDistance*/
{
	TransportTravelData = Data;
	TransportTravelData.bSpawnerDataSet = true;
}
