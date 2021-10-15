// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TItemDropAircraft.generated.h"

class USkeletalMeshComponent;
class ATItemDropZone;
class ATAirDropItem;
class UAudioComponent;

UCLASS()
class TANKS_API ATItemDropAircraft : public AActor
{
	GENERATED_BODY()

/**
 * Members
 */

protected:
	
	/** Mesh for Aircraft */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	/** Speed of Aircraft to fly across map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemDropAircraft")
	float Speed;

	/** Class for ItemToDrop  */
	UPROPERTY(EditDefaultsOnly, Category = "ItemDropClass");
	TSubclassOf<ATAirDropItem> ItemToDropClass;

	/** Location where Item droped should land  */
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = "ItemDropAircraft")
	FVector ItemDropZoneLocation;
	
	/** Current distance this ItemDropAircraft has traveled */
	UPROPERTY(VisibleAnywhere, Category = "ItemDropAircraft")
	float CurrentTravelDistance;

	/** Has Item been dropped */
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = "ItemDropAircraft")
	bool bItemDropped;

	/** Max distance this actor can travel, when CurrentTravelDistance is larger than MaxTravelDistance Destroy() will be called */
	UPROPERTY(Replicated, VisibleAnywhere, Category = "ItemDropAircraft")
	float MaxTravelDistance;

	/** Distance to travel before dropping item, when CurrentTravelDistance is larger than DistanceToDropItem item will be dropped */
	UPROPERTY(VisibleAnywhere, Category = "ItemDropAircraft")
	float DistanceToDropItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
	UAudioComponent* AudioComp;


 /**
  * Methods
  */
	
public:	
	// Sets default values for this actor's properties
	ATItemDropAircraft();

	/**
	 * [Server + Client] Set the flight path for this ItemDropAircraft actor
	 * 
	 * @param MaxTravelDistance - Destroy Actor after it moves farther than this distance from its spawn point
	 * @param DropZoneLocation - Location Item dropped will travel to
	 */
	void SetFlightPathParams(float MaxTravelDistance, FVector DropZoneLocation);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:

	virtual void BeginPlay() override;

	/** [Server] Drop Item, item will travel from this actors location to ItemDropZoneLocation */
	bool DropItem();


private:

	/** [Server] Check to see if item should be dropped */
	void CheckToDropItem();

	/** [Server + Client] Update the position of this actor */
	void UpdateAircraftLocation(float DeltaTime);


};
