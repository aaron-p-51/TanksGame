// Fill out your copyright notice in the Description page of Project Settings.


#include "TSpawnLocation.h"

// Engine Includes
#include "Components/SphereComponent.h"

// Game Includes

// Sets default values
ATSpawnLocation::ATSpawnLocation()
{
 	/*SpawnSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnSphere"));
	if (SpawnSphere)
	{
		SetRootComponent(SpawnSphere);
		SpawnSphere->SetSphereRadius(250.f);
		SpawnSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SpawnSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SpawnSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}*/

	bSpawnSpaceOpen = true;
}

// Called when the game starts or when spawned
void ATSpawnLocation::BeginPlay()
{
	Super::BeginPlay();

	/*if (SpawnSphere)
	{
		SpawnSphere->OnComponentBeginOverlap.AddDynamic(this, &ATSpawnLocation::OnSpawnSphereBeginOverlap);
		SpawnSphere->OnComponentEndOverlap.AddDynamic(this, &ATSpawnLocation::OnSpawnSphereEndOverlap);
	}*/
	
}

//void ATSpawnLocation::OnSpawnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	bSpawnSpaceOpen = false;
//}
//
//void ATSpawnLocation::OnSpawnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	bSpawnSpaceOpen = true;
//}

