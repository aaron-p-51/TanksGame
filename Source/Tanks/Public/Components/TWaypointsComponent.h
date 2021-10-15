// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TWaypointsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKS_API UTWaypointsComponent : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */

 public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
	TArray<AActor*> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
	bool bUseRandomSelection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waypoints")
	int32 StartingWaypointIndex;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoints")
	TArray<FVector> WaypointLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waypoints")
	int32 CurrentWaypointIndex;

 /**
  * Methods
  */
public:	
	// Sets default values for this component's properties
	UTWaypointsComponent();

	UFUNCTION(BlueprintCallable)
	FVector GetNextWaypoint();

	UFUNCTION(BlueprintCallable)
	FVector GetRandomWaypoint();

	UFUNCTION(BlueprintCallable)
	bool HasValidWaypoints();



protected:
	// Called when the game starts
	virtual void BeginPlay() override;


};
