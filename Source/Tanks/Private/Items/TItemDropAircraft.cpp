// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TItemDropAircraft.h"

// Engine Includes
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/AudioComponent.h"

// Game Includes
#include "Items/TAirItemSpawner.h"

// Sets default values
ATItemDropAircraft::ATItemDropAircraft()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	if (MeshComp)
	{
		SetRootComponent(MeshComp);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Hide props, looks like they are spinning
		MeshComp->HideBoneByName(TEXT("SK_Veh_American_Plane_Transport_Propeller_L"), EPhysBodyOp::PBO_None);
		MeshComp->HideBoneByName(TEXT("SK_Veh_American_Plane_Transport_Propeller_R"), EPhysBodyOp::PBO_None);
	}

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	if (AudioComp)
	{
		AudioComp->SetupAttachment(GetRootComponent());
	}

	SetReplicates(true);
}


void ATItemDropAircraft::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComp)
	{
		AudioComp->Play();
	}
}


// Called every frame
void ATItemDropAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetLocalRole() == ENetRole::ROLE_Authority && TotalTravelDistance > TransportTravelData.MaxTravelDistance)
	{
		Destroy();
	}
}


void ATItemDropAircraft::OnReachedTravelTransportDestination()
{
	if (AirItemSpawner && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AirItemSpawner->OnAircraftReachDropLocation(this);
	}
}

