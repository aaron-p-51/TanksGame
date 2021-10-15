// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TSpawnLocation.generated.h"

class USphereComponent;

UCLASS()
class TANKS_API ATSpawnLocation : public AActor
{
	GENERATED_BODY()
	

/**
 * Members
 */

protected:

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USphereComponent* SpawnSphere;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSpawnSpaceOpen;

 /**
  * Methods
  */


public:	
	// Sets default values for this actor's properties
	ATSpawnLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UFUNCTION()
	void OnSpawnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSpawnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

public:

	FORCEINLINE bool IsSpawnSpaceOpen() { return bSpawnSpaceOpen; }
	


};
