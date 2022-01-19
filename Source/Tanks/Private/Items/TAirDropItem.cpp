// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TAirDropItem.h"


// Game Includes
#include "Items/TAirItemSpawner.h"


// Sets default values
ATAirDropItem::ATAirDropItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ScaleRateChange = 1.f;
	SetReplicates(true);
}


// Called every frame
void ATAirDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TransportTravelData.bSpawnerDataSet && !bEnableMovement)
	{
		ShrinkAndDestroyActor(DeltaTime);
	}
}


void ATAirDropItem::ShrinkAndDestroyActor(float DeltaTime)
{
	float SizeChangeThisFrame = DeltaTime * ScaleRateChange;
	SetActorScale3D(GetActorScale3D() - SizeChangeThisFrame);

	if (GetLocalRole() == ENetRole::ROLE_Authority && GetActorScale3D().X < 0.1f)
	{
		Destroy();
	}
}


void ATAirDropItem::OnReachedTravelTransportDestination()
{
	if (AirItemSpawner && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AirItemSpawner->OnAirDropItemReachDropZone(this);
	}

	bEnableMovement = false;
}
