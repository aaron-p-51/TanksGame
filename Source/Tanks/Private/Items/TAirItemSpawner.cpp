// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TAirItemSpawner.h"

// Engine Includes
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathVectorConstants.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

#include "Items/TItemDropZone.h"
#include "Items/TItemDropAircraft.h"

// Sets default values
ATAirItemSpawner::ATAirItemSpawner()
{
	AirSpawnRadius = 1000.f;
	DropZoneEvaluationRate = 30.f;
	MaxDropZonesWithItems = 2;
	bAutoStart = false;
}


// Called when the game starts or when spawned
void ATAirItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetAllItemDropZones();
		if (bAutoStart)
		{
			StartAirItemSpawner();
		}
	}
}


void ATAirItemSpawner::StartAirItemSpawner()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimerTick, this, &ATAirItemSpawner::DefaultTimerTick, DropZoneEvaluationRate, true);
	}
}


void ATAirItemSpawner::StopAirItemSpawner()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimerTick);
	}
}


void ATAirItemSpawner::DefaultTimerTick()
{
	if (ItemDropZones.Num() - GetNumberOfEmptyDropZones() < MaxDropZonesWithItems)
	{
		DeployItemDropAircraft();
	}
}


void ATAirItemSpawner::GetAllItemDropZones()
{
	TArray<AActor*> DropZones;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ItemDropZoneClass, DropZones);

	for (auto DropZone : DropZones)
	{
		auto ItemDropZone = Cast<ATItemDropZone>(DropZone);
		if (ItemDropZone)
		{
			ItemDropZones.Add(ItemDropZone);
		}
	}
}


int32 ATAirItemSpawner::GetNumberOfEmptyDropZones()
{
	int32 EmptyDropZones = 0;
	for (auto Zone : ItemDropZones)
	{
		if (Zone->GetItemDropZoneState() == EItemDropZoneState::EIDZS_Empty)
		{
			++EmptyDropZones;
		}
	}

	return EmptyDropZones;
}


void ATAirItemSpawner::DeployItemDropAircraft()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		// Choose a random available Drop Zone
		ATItemDropZone* DropZone = GetRandomEmptyDropZone();
		if (DropZone)
		{
			SetFlightPathToItemDropZone(DropZone);
		}
	}
}


ATItemDropZone* ATAirItemSpawner::GetRandomEmptyDropZone()
{
	if (ItemDropZones.Num() <= 0) return nullptr;

	// Get all drop zones that are available, ie empty state
	TArray<ATItemDropZone*> AvailableDropZones;
	for (auto Zone : ItemDropZones)
	{
		if (Zone->GetItemDropZoneState() == EItemDropZoneState::EIDZS_Empty)
		{
			AvailableDropZones.Add(Zone);
		}
	}

	if (AvailableDropZones.Num() <= 0) return nullptr;

	int32 AvailableDropZoneIndex = FMath::RandRange(0, AvailableDropZones.Num() - 1);
	return AvailableDropZones[AvailableDropZoneIndex];
}


void ATAirItemSpawner::SetFlightPathToItemDropZone(ATItemDropZone* DropZone)
{
	if (!DropZone)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoDropZone"));
		return;
	}

	// Get Position above drop zone
	FVector AirDropLocation = DropZone->GetActorLocation();
	AirDropLocation.Z = GetActorLocation().Z;
	

	// Get random spawn Location on around the AirItemSpawner location, at distance defined by AirSpawnRadius
	FVector SpawnLocation;
	float RandomUnitCircleAngle = FMath::FRandRange(0, 2.f * PI);
	SpawnLocation.X = AirSpawnRadius * FMath::Cos(RandomUnitCircleAngle);
	SpawnLocation.Y = AirSpawnRadius * FMath::Sin(RandomUnitCircleAngle);
	SpawnLocation.Z = 0.f;
	
	// Convert SpawnLocation offset for this Actors location
	SpawnLocation += GetActorLocation();

	// Get direction to face the AirDropLocation from SpawnLocation
	FRotator SpawnRotation = (AirDropLocation - SpawnLocation).Rotation();

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	SpawnItemDropAircraft(SpawnTransform, DropZone);
}


void ATAirItemSpawner::SpawnItemDropAircraft(FTransform SpawnTransform, ATItemDropZone* DropZone)
{
	if (!DropZone || !ItemDropAircraftClass) return;

	ATItemDropAircraft* Aircraft = Cast<ATItemDropAircraft>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
		ItemDropAircraftClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

	if (Aircraft)
	{
		Aircraft->SetFlightPathParams(AirSpawnRadius * 2.f, DropZone->GetActorLocation());
		DropZone->SetItemDropZoneState(EItemDropZoneState::EIDZS_ItemInbound);
		UGameplayStatics::FinishSpawningActor(Aircraft, SpawnTransform);
	}
}


