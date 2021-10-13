// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TWaypointsComponent.h"

// Sets default values for this component's properties
UTWaypointsComponent::UTWaypointsComponent()
{

}


// Called when the game starts
void UTWaypointsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get the Waypoint Actors and get there location, location will be used for all operations
	for (auto Waypoint : Waypoints)
	{
		if (Waypoint != nullptr)
		{
			WaypointLocations.AddUnique(Waypoint->GetActorLocation());
		}
	}
}


FVector UTWaypointsComponent::GetNextWaypoint()
{
	++CurrentWaypointIndex;
	if (CurrentWaypointIndex >= WaypointLocations.Num())
	{
		CurrentWaypointIndex = 0;
	}

	return WaypointLocations[CurrentWaypointIndex];
}


FVector UTWaypointsComponent::GetRandomWaypoint()
{
	CurrentWaypointIndex = FMath::RandRange(0, WaypointLocations.Num() - 1);
	return WaypointLocations[CurrentWaypointIndex];
}

bool UTWaypointsComponent::HasValidWaypoints()
{
	return WaypointLocations.Num() > 0;
}




