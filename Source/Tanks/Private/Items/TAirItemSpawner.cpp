// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TAirItemSpawner.h"

// Engine Includes
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathVectorConstants.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"


// Game Includes
#include "Items/TItemDropZone.h"
#include "Items/TItemDropAircraft.h"
#include "Items/TPickupItem.h"
#include "Items/TAirDropItem.h"


// Sets default values
ATAirItemSpawner::ATAirItemSpawner()
{
	AirSpawnRadius = 1000.f;
	DropZoneEvaluationRate = 30.f;
	MaxDropZonesWithItems = 2;
	bAutoStart = true;
}


// Called when the game starts or when spawned
void ATAirItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetAllItemDropZones();
		BP_SetDebugLocations();
		CalculateBaseItemSpawnScale();
		if (bAutoStart)
		{
			StartAirItemSpawner();
		}
	}
}


void ATAirItemSpawner::GetAllItemDropZones()
{
	const FVector AirItemSpawnerCenter = GetActorLocation();
	TArray<AActor*> DropZones;
	const float MaxItemDropZoneDistance = FMath::Sqrt(FMath::Square(AirItemSpawnerCenter.Z) + FMath::Square(AirSpawnRadius));

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ItemDropZoneClass, DropZones);

	for (auto& DropZone : DropZones)
	{
		ATItemDropZone* ItemDropZone = Cast<ATItemDropZone>(DropZone);
		if (ItemDropZone)
		{
			// Make sure DropZone is inside of AirSpawnRadius
			const FVector ItemDropZoneLocation = ItemDropZone->GetActorLocation();
			if (ItemDropZoneLocation.Z < AirItemSpawnerCenter.Z && MaxItemDropZoneDistance > FVector::Distance(ItemDropZoneLocation, AirItemSpawnerCenter))
			{
				//ItemDropZone->SetAirItemSpawner(this);
				ItemDropZones.Add(ItemDropZone);
			}
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


/**************************************************************************/
/* Aircraft Spawn */
/**************************************************************************/
void ATAirItemSpawner::DefaultTimerTick()
{
	int32 EmptyDropZones = GetNumberOfEmptyDropZones();
	if ((bFillAllItemDropZones && EmptyDropZones != 0) || ItemDropZones.Num() - EmptyDropZones < MaxDropZonesWithItems)
	{
		DeployItemDropAircraft();
	}
}


int32 ATAirItemSpawner::GetNumberOfEmptyDropZones()
{
	int32 EmptyDropZones = 0;
	for (const auto& Zone : ItemDropZones)
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
		ATItemDropZone* DropZone = GetRandomEmptyDropZone();
		if (ItemDropZones.Num() > 0)
		{
			if (DropZone)
			{
				FTransform AircraftSpawnTransform = GetAircraftSpawnTransform(DropZone->GetActorLocation());
				SpawnItemDropAircraft(AircraftSpawnTransform, DropZone);
			}
		}
	}
}


ATItemDropZone* ATAirItemSpawner::GetRandomEmptyDropZone()
{
	if (ItemDropZones.Num() > 0)
	{
		// Get all drop zones that are available, ie empty state
		TArray<ATItemDropZone*> AvailableDropZones;
		for (auto Zone : ItemDropZones)
		{
			if (Zone->GetItemDropZoneState() == EItemDropZoneState::EIDZS_Empty)
			{
				AvailableDropZones.Add(Zone);
			}
		}

		// Choose a random drop zone with with empty state found above
		if (AvailableDropZones.Num() > 0)
		{
			int32 AvailableDropZoneIndex = FMath::RandRange(0, AvailableDropZones.Num() - 1);
			return AvailableDropZones[AvailableDropZoneIndex];
		}
	}

	return nullptr;
}


FTransform ATAirItemSpawner::GetAircraftSpawnTransform(const FVector& ItemDropZoneLocation) const
{	
	// Get a random point on circle circumference with radius AirSpawnRadius
	const float RandomUnitCircleAngle = FMath::FRandRange(0, 2.f * PI);
	FVector SpawnLocation(AirSpawnRadius * FMath::Cos(RandomUnitCircleAngle), AirSpawnRadius * FMath::Sin(RandomUnitCircleAngle), 0.f);
	
	// Convert SpawnLocation offset for this Actors location
	SpawnLocation += GetActorLocation();
	
	// Location directly above ItemDropZoneLocation. At same height as this actor
	FVector AirDropLocation = GetAirDropLocation(ItemDropZoneLocation);   

	// Rotation if from circle circumference facing AirDropLocation (located towards center of circle)
	FRotator SpawnRotation = (AirDropLocation - SpawnLocation).Rotation();

	return FTransform(SpawnRotation, SpawnLocation);
}


FVector ATAirItemSpawner::GetAirDropLocation(const FVector& ItemDropZoneLocation) const
{
	return FVector(ItemDropZoneLocation.X, ItemDropZoneLocation.Y, GetActorLocation().Z);
}


void ATAirItemSpawner::SpawnItemDropAircraft(FTransform SpawnTransform, ATItemDropZone* DropZone)
{
	if (DropZone && ItemDropAircraftClass)
	{
		ATItemDropAircraft* Aircraft = Cast<ATItemDropAircraft>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
			ItemDropAircraftClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

		if (Aircraft)
		{
			// Calculate Aircrafts travel to location. This will be other size of circle defined by this actors location and AirSpawnRadius
			const FVector TravelToLocation = SpawnTransform.GetLocation() + (SpawnTransform.GetRotation().GetForwardVector() * AirSpawnRadius * 2);

			FVector ItemDropLocation = DropZone->GetActorLocation();
			ItemDropLocation.Z = SpawnTransform.GetLocation().Z;

			const float DistanceToDropZone = (ItemDropLocation - SpawnTransform.GetLocation()).Size();
			const float MaxTravelDistance = AirSpawnRadius * 2;

			FTransportTravelData Data(TravelToLocation, DistanceToDropZone, MaxTravelDistance);
			Aircraft->SetAirItemSpawnerData(DropZone, this, Data);

			if (UGameplayStatics::FinishSpawningActor(Aircraft, SpawnTransform))
			{
				DropZone->SetItemDropZoneState(EItemDropZoneState::EIDZS_ItemInbound);
			}
		}
	}
}


/**************************************************************************/
/* AirDropItem Spawn */
/**************************************************************************/
void ATAirItemSpawner::OnAircraftReachDropLocation(ATItemDropAircraft* Aircraft)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && Aircraft && AirDropItemClass)
	{
		FTransform ItemDropSpawnTransform(Aircraft->GetActorRotation(), Aircraft->GetActorLocation());
		SpawnAirDropItem(ItemDropSpawnTransform, Aircraft->GetAssignedItemDropZone());
	}
}


void ATAirItemSpawner::SpawnAirDropItem(const FTransform& SpawnTransform, ATItemDropZone* DropZone)
{
	if (DropZone)
	{
		ATAirDropItem* AirDropItem = Cast<ATAirDropItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
			AirDropItemClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

		if (AirDropItem)
		{

			const float DistanceToTransportDestination = FVector::Distance(DropZone->GetActorLocation(), SpawnTransform.GetLocation());
			const float MaxTravelDistance = DistanceToTransportDestination * 2.f;

			FTransportTravelData Data(DropZone->GetActorLocation(), DistanceToTransportDestination, MaxTravelDistance);
			AirDropItem->SetAirItemSpawnerData(DropZone, this, Data);

			// If item fails to spawn the set DropZone back to empty state so it can be selected again
			if (!UGameplayStatics::FinishSpawningActor(AirDropItem, SpawnTransform))
			{
				DropZone->SetItemDropZoneState(EItemDropZoneState::EIDZS_Empty);
			}
		}
	}
}


/**************************************************************************/
/* Item Spawn */
/**************************************************************************/
void ATAirItemSpawner::OnAirDropItemReachDropZone(ATAirDropItem* Item)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && Item && Item->GetAssignedItemDropZone() && ItemDropZones.Num() > 0)
	{
		ATItemDropZone* DropZone = Item->GetAssignedItemDropZone();

		// Resets PickupItems RandomSpawnScale
		CalculateBaseItemSpawnScale();

		// Adjust PickupItems RandomSpawnScale for items already in level wiating for player pickup
		AdjustItemRandomSpawnScale();

		// Set PickupItems RandomSpawnScaleRange. Get all encomposing range of all PickupItems
		TRange<float> SpawnRanges = SetItemSpawnRandomSpawnScaleRange();

		// Select Item, entry from PickupItems, to spawn
		TTuple<EItemType, FPickUpItemData> ItemToSpawn = GetRandomItemToSpawn(SpawnRanges);

		FTransform SpawnTransform(DropZone->GetActorRotation(), DropZone->GetItemPickupSpawnPoint());
		AActor* SpawnedItemActor = SpawnItem(ItemToSpawn, SpawnTransform);

		ATPickupItem* PickupItem = Cast<ATPickupItem>(SpawnedItemActor);
		if (PickupItem)
		{
			PickupItem->AirItemSpawner = this;
			PickupItem->ItemDropZone = DropZone;

			DropZone->SetCurrentPickupItemInDropZone(PickupItem);
			DropZone->SetItemDropZoneState(EItemDropZoneState::EIDZS_ItemInDropZone);

			FPickUpItemData* PickupData = PickupItems.Find(ItemToSpawn.Key);
			if (PickupData)
			{
				PickupData->AmountOnMap += 1;
			}
		}
	}
}


void ATAirItemSpawner::CalculateBaseItemSpawnScale()
{
	float TotalSpawnChance = 0;
	for (const auto& PickupItem : PickupItems)
	{
		TotalSpawnChance += PickupItem.Value.SpawnWeight;
	}

	if (!FMath::IsNearlyZero(TotalSpawnChance))
	{
		for (auto& PickupItem : PickupItems)
		{
			PickupItem.Value.RandomSpawnScale = PickupItem.Value.SpawnWeight / TotalSpawnChance;
		}
	}
}


void ATAirItemSpawner::AdjustItemRandomSpawnScale()
{
	if (ItemDropZones.Num() > 0)
	{
		// For every item in PickupItem map calculate the percent of ItemDropZones have that item currently spawned
		for (const auto& PickupItem : PickupItems)
		{
			const float ItemPercentCurrentySpawned = (float)PickupItem.Value.AmountOnMap / ItemDropZones.Num();
			const int32 TotalWeightedPickupItems = GetTotalWeightedPickupItems() - 1;
			if (TotalWeightedPickupItems > 0)
			{

				// Decrease the spawn percent of an item spawning based on percent of the same type of items
				// currently spawned. Increase the percent of all other items spawning.
				for (auto& AdjustPickupItem : PickupItems)
				{
					if (AdjustPickupItem.Value.SpawnWeight != 0)
					{
						if (AdjustPickupItem.Key == PickupItem.Key)
						{
							AdjustPickupItem.Value.RandomSpawnScale -= ItemPercentCurrentySpawned;
						}
						else
						{
							AdjustPickupItem.Value.RandomSpawnScale += (ItemPercentCurrentySpawned / TotalWeightedPickupItems);
						}
					}
				}
			}
		}
	}
}


TRange<float> ATAirItemSpawner::SetItemSpawnRandomSpawnScaleRange()
{
	float LastRangeUpperBound = 0;
	for (auto& PickupItem : PickupItems)
	{
		if (PickupItem.Value.RandomSpawnScale > 0.f)
		{
			PickupItem.Value.RandomSpawnScaleRange = TRange<float>(LastRangeUpperBound, LastRangeUpperBound + PickupItem.Value.RandomSpawnScale);
			LastRangeUpperBound = LastRangeUpperBound + PickupItem.Value.RandomSpawnScale;
		}
	}

	return TRange<float>(0.f, LastRangeUpperBound);
}


TTuple<EItemType, FPickUpItemData> ATAirItemSpawner::GetRandomItemToSpawn(const TRange<float>& RandomSpawnRange) const
{
	EItemType ItemType = EItemType::EIT_NotSpecified;
	FPickUpItemData PickUpITemData;
	const float RandVal = FMath::RandRange(RandomSpawnRange.GetLowerBoundValue(), RandomSpawnRange.GetUpperBoundValue());

	for (const auto& PickupItem : PickupItems)
	{
		if (PickupItem.Value.RandomSpawnScaleRange.Contains(RandVal))
		{
			return TTuple<EItemType, FPickUpItemData>(PickupItem.Key, PickupItem.Value);
		}
	}

	return TTuple<EItemType, FPickUpItemData>();
}


AActor* ATAirItemSpawner::SpawnItem(const TTuple<EItemType, FPickUpItemData>& Item, const FTransform& SpawnTransform) const
{
	ATPickupItem* PickupItem = Cast<ATPickupItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
		Item.Value.PickupItemClass, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

	if (PickupItem)
	{
		return UGameplayStatics::FinishSpawningActor(PickupItem, SpawnTransform);
	}
	
	return nullptr;
}


int32 ATAirItemSpawner::GetTotalWeightedPickupItems() const
{
	int32 TotalWeightedPickupItems = 0;
	for (const auto& PickupItem : PickupItems)
	{
		if (PickupItem.Value.SpawnWeight > 0)
		{
			++TotalWeightedPickupItems;
		}
	}

	return TotalWeightedPickupItems;
}


void ATAirItemSpawner::OnPickupItemPickupUp(ATPickupItem* Item)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && Item)
	{
		ATItemDropZone* DropZone = Item->ItemDropZone;
		if (DropZone)
		{
			DropZone->SetCurrentPickupItemInDropZone(nullptr);
			DropZone->SetItemDropZoneState(EItemDropZoneState::EIDZS_Empty);
		}
		
		FPickUpItemData* PickupData = PickupItems.Find(Item->GetItemType());
		if (PickupData)
		{
			PickupData->AmountOnMap = FMath::Max<int32>(0, PickupData->AmountOnMap - 1);
		}
	}
}



