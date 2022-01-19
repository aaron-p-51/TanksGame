// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TPickupItem.h"

// Engine Includes
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Net/UnrealNetwork.h"

// Game Includes
#include "Pawns/TPlayerTank.h"
#include "Items/TAirItemSpawner.h"
#include "Items/TItemDropZone.h"

// Sets default values
ATPickupItem::ATPickupItem()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	SphereComp->SetSphereRadius(75.f);


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(GetRootComponent());
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComp->CastShadow = 0;
	

	RotatingMovementComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComp"));

	bItemPickedUp = false;

	SetReplicates(true);

}


// Called when the game starts or when spawned
void ATPickupItem::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ENetRole::ROLE_Authority && SphereComp)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ATPickupItem::OnSphereCompBeginOverlap);
	}
}


void ATPickupItem::OnSphereCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATPlayerTank* PlayerTank = Cast<ATPlayerTank>(OtherActor);
	if (PlayerTank)
	{
		bItemPickedUp = PlayerTank->PickupItem(ItemType, ItemQuantity);
		if (bItemPickedUp)
		{
			if (AirItemSpawner)
			{
				AirItemSpawner->OnPickupItemPickupUp(this);
			}

			MulticastHealthPickupUp();
			SetLifeSpan(1.f);	
		}
	}
}


void ATPickupItem::OnItemPickedUp()
{
	if (MeshComp)
	{
		MeshComp->SetVisibility(false, true);
	}
	if (SphereComp)
	{
		SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SpawnPickupFX();
}


void ATPickupItem::SpawnPickupFX() const
{
	if (PickupParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticles, GetActorLocation(), GetActorRotation(), true);
	}
}


void ATPickupItem::MulticastHealthPickupUp_Implementation()
{
	bItemPickedUp = true;
	OnItemPickedUp();
}
