// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPickupItem.generated.h"


class USphereComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class TANKS_API ATPickupItem : public AActor
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	/** Collision Component  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	/** Mesh Comp */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* PickupSoundEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	bool bPlaySound2D;


 /**
  * Methods
  */

public:	
	// Sets default values for this actor's properties
	ATPickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** [Server] Item will be picked up on SphereCompBeginOverlap */
	UFUNCTION()
	void OnSphereCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
