// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TItemDropZone.generated.h"


class UBoxComponent;

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


UCLASS()
class TANKS_API ATItemDropZone : public AActor
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	/** Current state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	EItemDropZoneState ItemDropZoneState;

	/** Actor to spawn when ATAirDropItem collides with this DropZone */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<AActor> PickupIcon;

	/** Collision to detect ATAirDropItem */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UBoxComponent* BoxCollisionComponent;

	/** Offset to spawn PickupIcon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemPickupSpawnPoint;

	/** Current Pickup in DropZone, should be of type PickupIcon */
	UPROPERTY(VisibleAnywhere)
	AActor* CurrentPickupItemInDropZone;

 /**
  * Methods
  */
	
public:	
	//// Sets default values for this actor's properties
	ATItemDropZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** [Server] Collision of ATAirDropItem will spawn PickupIcon, with ItemPickupSpawnPoint offset */
	UFUNCTION()
	void OnBoxCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	

	/** Get the curret DropZoneState, will update and check for valid CurrentPickupItemInDropZone */
	EItemDropZoneState GetItemDropZoneState();

	/** Set current State */
	void SetItemDropZoneState(EItemDropZoneState NewState);

};
