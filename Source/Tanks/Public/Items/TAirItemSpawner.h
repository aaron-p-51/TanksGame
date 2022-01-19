// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Range.h"
#include "TTanksTypes.h"
#include "GameFramework/Actor.h"
#include "TAirItemSpawner.generated.h"

class ATItemDropZone;
class ATItemDropAircraft;
class ATPickupItem;
class ATAirDropItem;

USTRUCT(BlueprintType)
struct FPickUpItemData
{
	GENERATED_BODY()

	/** Pick Item Class to spawn */
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATPickupItem> PickupItemClass;

	/**
	 * Weight, desirability of this item to spawn over other items. The higher the spawn weight the more
	 * change for this item to spawn and take up more ItemDropZone locations
	 */
	UPROPERTY(EditAnywhere)
	int32 SpawnWeight = 1;

	/**
	 * Assigned based on SpawnWeight and current number of this item already in spawn location. The more of this item
	 * in spawn locations the lower this number will be. The higher this number the greater the chance for it to be randomly
	 * selected as the next item to spawn
	 */
	UPROPERTY(VisibleAnywhere)
	float RandomSpawnScale = 0.f;

	/**
	 * Upper and lower bounds of of the RandomSpawnScale for all items. Will be in relation to other Item's RandomSpawnScale.
	 * For example, Item1.RandomSpawnScale = 0.25, Item2.RandomSpawnScale = 0.3, Item3.RandomSpawnScale = 0.15. The RandomSPawnScaleRange
	 * for each item will be set as follows. Item1.RandomSpawnScaleRange = (0, 0.25), Item2.RandomSpawnScaleRange = (0.3, 0.55),
	 * Item3.RandomSpawnScaleRange = (0.55, 0.70)
	 */
	TRange<float> RandomSpawnScaleRange = TRange<float>(0.f, 0.f);

	/** Amount of this item currently spawned and ready to be picked up by a player */
	UPROPERTY(VisibleAnywhere)
	int32 AmountOnMap = 0;
};


/**
 * Air Item spawn system base class. This class is main class for coordinating and spawning items in the map.
 * 
 */
UCLASS()
class TANKS_API ATAirItemSpawner : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */
private:

	/**************************************************************************/
	/* Configuration */
	/**************************************************************************/
	
	/** Items spawned from the AirITemSpawner system.  */
	UPROPERTY(EditAnywhere, Category = "Configuration|Items")
	TMap<EItemType, FPickUpItemData> PickupItems;


	/** Class for Item Drop Zones, Locations on map where items will be dropped to */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|DropZones")
	TSubclassOf<ATItemDropZone> ItemDropZoneClass;

protected:

	/**
	 * Radius from where to spawn ItemDropAircraft, should be large enough to minimize
	 * players seeing aircraft spawning and destroying. ItemDropZones will not be used
	 * if outside this radius
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float AirSpawnRadius;

	/** Array of all DropZones on map that are of type ATItemDropZOne */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Configuration|DropZones")
	TArray<ATItemDropZone*> ItemDropZones;

private:

	/** Class for Aircraft to carry item to DropZone */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Aircraft")
	TSubclassOf<ATItemDropAircraft> ItemDropAircraftClass;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration|AirDropItem")
	TSubclassOf<ATAirDropItem> AirDropItemClass;


	/** Time between DropZone evaluation cycles. Each interval will only spawn at most one ATItemDropAircraft */
	UPROPERTY(EditAnywhere, Category = "Configuration")
	float DropZoneEvaluationRate;

	/** Fill all ItemDropZones with items. Will not fill all at once. */
	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bFillAllItemDropZones;

	/** Do not spawn for ItemDropAircraft if there are this many or more items in DropZOnes  */
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "!bFillAllItemDropZones"))
	int32 MaxDropZonesWithItems;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool bAutoStart;

	/** Timer for re evaluate DropZones and spawn aircraft to drop items */
	FTimerHandle TimerHandle_DefaultTimerTick;


 /**
  * Methods
  */

public:	
	// Sets default values for this actor's properties
	ATAirItemSpawner();

	/** [Server] Start the AirItemSpawner system */
	UFUNCTION(BlueprintCallable)
	void StartAirItemSpawner();

	/** [Server] Stop the AirItemSpawner system */
	UFUNCTION(BlueprintCallable)
	void StopAirItemSpawner();

	/** [Server] Call when item is picked up by player. Item is the actual ATPickupItem the player pickup up */
	void OnPickupItemPickupUp(ATPickupItem* Item);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** For Debug. Caches ItemDrop locations for debug draw */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetDebugLocations();


private:

	/**************************************************************************/
	/* State */
	/**************************************************************************/

	/** [Server] Get number of ItemDropZones with EIDZS_Empty ItemDropZoneState. Will only be valid on server*/
	int32 GetNumberOfEmptyDropZones();

	/** [Server] Get all ItemDropZones, store in ItemDropZones TArray  */
	UFUNCTION(BlueprintCallable)
	void GetAllItemDropZones();

	/** Get number of entries in PickupItmes that have SpawnWeight > 0 */
	int32 GetTotalWeightedPickupItems() const;


	/**************************************************************************/
	/* Aircraft Spawn */
	/**************************************************************************/

	/**
	 * [Server] Evaluate DropZones for ItemDropZoneState. If conditions of bFillAllItemDropZones and MaxDropZonesWithItems
	 *	are meet then spawn an ItemDropAircraft
	 */
	void DefaultTimerTick();

	/** [Server] DeployItemDropAircraft to random available ItemDropZone */
	UFUNCTION(BlueprintCallable)
	void DeployItemDropAircraft();

	/**
	 * [Server] Calculate the SpawnTrasnform for a new ATItemDropAircraft. Location of returned transform will be randomly located
	 * on circle defined by this actors location and AirSpawnRadius. The ration will ItemDropZoneLocation at same Z location as this actor 
	 */
	FTransform GetAircraftSpawnTransform(const FVector& ItemDropZoneLocation) const;

	/** [Server] Spawn ItemDropAircraft at SpawnTransform, Set Aircraft flightpath to DropZone */
	void SpawnItemDropAircraft(FTransform SpawnTransform, ATItemDropZone* DropZone);

	/** [Server] Get a random ItemDropZone with an EIDZS_Empty ItemDropZoneState */
	ATItemDropZone* GetRandomEmptyDropZone();

	/** Get the location where Aircraft will drop  */
	FVector GetAirDropLocation(const FVector& ItemDropZoneLocation) const;


	/**************************************************************************/
	/* AirDropItem Spawn */
	/**************************************************************************/

public:

	/** [Server] Call when Aircraft spawned from SpawnItemDropAircraft reaches its assigned DropZone */
	void OnAircraftReachDropLocation(ATItemDropAircraft* Aircraft);

private:

	/** [Server] Spawn an AirDropItem at SpawnTransform. Assign its DropZone to DropZone param */
	void SpawnAirDropItem(const FTransform& SpawnTransform, ATItemDropZone* DropZone);


	/**************************************************************************/
	/* Item Spawn */
	/**************************************************************************/

public:

	/** [Server] Spawn an item with valid entry from PickupItems Map. Will spawn item at DropZone */
	void OnAirDropItemReachDropZone(ATAirDropItem* Item);

	/** Calculate and set all PickupItems entries ItemSpawnScale. Based on SpawnWeight of all entries  */
	void CalculateBaseItemSpawnScale();

	/**
	 * Calculate and set all PickupItems entries RandomSpawnScale. Based on number of current items of each
	 * PickupItems entry already spawned and ready to be picked up by a player
	 */
	void AdjustItemRandomSpawnScale();

	/** Calculate and set all PickupItem entries RandomSpawnScaleRange. Returns to total range, min and max value of all entries */
	TRange<float> SetItemSpawnRandomSpawnScaleRange();

	/**
	 * Select a random entry form PickupItems to spawn. The probability of item selected to spawn is based on PickupItem RandomSpawnScaleRange
	 * and how much that range fills param RandomSpawnRange. Returns entry from PickupItems to spawn.
	 */
	TTuple<EItemType, FPickUpItemData> GetRandomItemToSpawn(const TRange<float>& RandomSpawnRange) const;

	/** [Server] Spawn Item at SpawnTransform. Return spawned Actor */
	AActor* SpawnItem(const TTuple<EItemType, FPickUpItemData>& Item, const FTransform& SpawnTransform) const;

};
