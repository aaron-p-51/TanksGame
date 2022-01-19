// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "GameFramework/Actor.h"
#include "TPickupItem.generated.h"

/** Forward Declarations */
class USphereComponent;
class UStaticMeshComponent;
class USoundBase;
class URotatingMovementComponent;
class UParticleSystem;
class ATAirItemSpawner;
class ATItemDropZone;

/**
 * Base class for Items that are picked up by ATPlayerTanks on map. Will add ItemQuantity of ItemType
 * to ATPlayerTanks who collects this ATPicupItem
 */
UCLASS()
class TANKS_API ATPickupItem : public AActor
{
	GENERATED_BODY()

/**
 * Members
 */

private:

	/*************************************************************************/
	/* Components */
	/*************************************************************************/

	/** Collision Component will trigger item pickup on overlap */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	URotatingMovementComponent* RotatingMovementComp;


	/*************************************************************************/
	/* Components */
	/*************************************************************************/

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	int32 ItemQuantity;
	

	/*************************************************************************/
	/* FX */
	/*************************************************************************/
	UPROPERTY(EditAnywhere, Category = "FX")
	USoundBase* PickupSoundEffects;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* PickupParticles;

	/*************************************************************************/
	/* State */
	/*************************************************************************/

	UPROPERTY(VisibleAnywhere)
	bool bItemPickedUp;

public:

	UPROPERTY(VisibleAnywhere)
	ATAirItemSpawner* AirItemSpawner;

	UPROPERTY(VisibleAnywhere)
	ATItemDropZone* ItemDropZone;



 /**
  * Methods
  */

public:	
	// Sets default values for this actor's properties
	ATPickupItem();

	EItemType GetItemType() const { return ItemType; }

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:

	/** [Server] Item will be picked up on SphereCompBeginOverlap */
	UFUNCTION()
	void OnSphereCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/** [Server + Client] Disable all collisions and hit MeshComp visibility */
	void OnItemPickedUp();

	/** [Client] Spawn Pickup FX when item it successfully picked up */
	void SpawnPickupFX() const;

	/** Item is picked up.  */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthPickupUp();

};
