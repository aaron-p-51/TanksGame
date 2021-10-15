// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TItemPickupLocation.h"

// Engine Includes
#include "Kismet/GameplayStatics.h"

// Sets default values
ATItemPickupLocation::ATItemPickupLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATItemPickupLocation::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ATItemPickupLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

