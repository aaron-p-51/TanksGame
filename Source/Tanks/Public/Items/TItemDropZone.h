// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "GameFramework/Actor.h"
#include "TItemDropZone.generated.h"

class ATPickupItem;
class UBoxComponent;
class ATAirItemSpawner;

/**
 * States of Items in DropZone:
 * 
 * EIDZS_Empty - No item in drop zone, not ATItemDropAircraft inbound to drop item
 * 
 * EIDZS_ItemInbound - ATITemDropAircraft has been deployed to this ATItemDropZone
 *					   Item not yet in DropZone, may be falling from aircraft
 * 
 * EIDZS_ItemInDropZOne - Item is in drop zone, can be picked up by player
 */
UENUM(BlueprintType)
enum class EItemDropZoneState : uint8
{
	EIDZS_Empty				UMETA(DisplayName="Empty"),
	EIDZS_ItemInbound		UMETA(DisplayName="Inbound"),
	EIDZS_ItemInDropZone	UMETA(DisplayName="InDropZone"),

	EIDZS_DefaultMax		UMETA(DisplayName="MAX")
};


/**
 * Base class for Item Drop Zones.
 */
UCLASS()
class TANKS_API ATItemDropZone : public AActor
{
	GENERATED_BODY()


/**
 * Members
 */

private:

	/** Offset to spawn PickupIcon */
	UPROPERTY(EditAnywhere, Category = "Item")
	USceneComponent* ItemPickupSpawnPoint;

	/** Current state */
	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemDropZoneState ItemDropZoneState;

	/** Current PickupItem in DropZone. */
	UPROPERTY(VisibleAnywhere)
	AActor* CurrentPickupItemInDropZone;


 /**
  * Methods
  */
	
public:	
	
	// Sets default values for this actor's properties
	ATItemDropZone();

	/** Getter and setter CurrentPickupItemInDropZone */
	void SetCurrentPickupItemInDropZone(AActor* Actor);
	AActor* GetCurrentPickupItemInDropZone() const { return CurrentPickupItemInDropZone; }
	
	/** Getter and setter for CurrentPickupItemInDropZone */
	EItemDropZoneState GetItemDropZoneState() const { return ItemDropZoneState; }
	void SetItemDropZoneState(EItemDropZoneState NewState);

	/** Get the ItemPickupSpawnPoint location in world space */
	FVector GetItemPickupSpawnPoint() const;

};
