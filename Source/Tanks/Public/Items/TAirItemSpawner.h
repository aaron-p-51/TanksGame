// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAirItemSpawner.generated.h"

class ATItemDropZone;
class ATItemDropAircraft;

UCLASS()
class TANKS_API ATAirItemSpawner : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */
protected:

	/** Class for Item Drop Zones, Locations on map where items will be dropped to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ItemSpawning")
	TSubclassOf<ATItemDropZone> ItemDropZoneClass;

	/** Array of all DropZones on map that are of type ATItemDropZOne */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemSpawning")
	TArray<ATItemDropZone*> ItemDropZones;

	/** Class for Aircraft to carry item to DropZone */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemSpawning")
	TSubclassOf<ATItemDropAircraft> ItemDropAircraftClass;

	/** Radius from where to spawn ItemDropAircraft, should be large enough to minimize
	 *  players seeing aircraft spawning and destroying
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawning")
	float AirSpawnRadius;

	/** Timer for re evaluate DropZones and spawn aircraft to drop items */
	FTimerHandle TimerHandle_DefaultTimerTick;

	/** Time between DropZone evaluation cycles */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawning")
	float DropZoneEvaluationRate;

	/** Do not spawn for ItemDropAircraft if there are this many or more items in DropZOnes  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawning")
	int32 MaxDropZonesWithItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawning")
	bool bAutoStart;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** [Server] Evaluate DropZones spawns for item status. Spawn ItemDropAircraft  */
	void DefaultTimerTick();

	/** [Server] DeployItemDropAircraft to random Available ItemDropZone */
	UFUNCTION(BlueprintCallable)
	void DeployItemDropAircraft();

	/** [Server] Get all ItemDropZones, store in ItemDropZones TArray  */
	UFUNCTION(BlueprintCallable)
	void GetAllItemDropZones();

	/** [Server] Get number of ItemDropZones with EIDZS_Empty ItemDropZoneState*/
	int32 GetNumberOfEmptyDropZones();

	/** [Server] Get a random ItemDropZone with an EIDZS_Empty ItemDropZoneState */
	ATItemDropZone* GetRandomEmptyDropZone();

	/** [Server] Set flight path from random point on AirSpawnRadius over DropZone  */
	void SetFlightPathToItemDropZone(ATItemDropZone* DropZone);

	/** [Server] Spawn ItemDropAircraft at SpawnTransform, Set Aircraft flightpath to DropZone */
	void SpawnItemDropAircraft(FTransform SpawnTransform, ATItemDropZone* DropZone);

};
