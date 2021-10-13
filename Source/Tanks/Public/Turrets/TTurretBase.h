// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTurretBase.generated.h"


class USphereComponent;


UCLASS(Abstract, Blueprintable)
class TANKS_API ATTurretBase : public AActor
{
	GENERATED_BODY()
	
/**
 * Members
 */

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	uint8 TurretTeamID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensing");
	USphereComponent* SphereSensingComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensing")
	float SensingAngle;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BaseMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USceneComponent* FirePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sensing")
	TSubclassOf<APawn> TrackingPawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Sensing")
	APawn* CurrentTrackedPawn;



 /**
  * Methods
  */
public:	
	// Sets default values for this actor's properties
	ATTurretBase();

	FORCEINLINE APawn* GetCurrentTrackedPawn() const { return CurrentTrackedPawn; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	APawn* LookForHostileTarget();

	bool TargetWithinMaxAngle(const FVector& TargetLocation) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
