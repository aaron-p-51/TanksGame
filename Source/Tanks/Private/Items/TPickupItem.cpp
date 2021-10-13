// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TPickupItem.h"

// Engine Includes
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Game Includes
#include "Pawns/TPlayerTank.h"

// Sets default values
ATPickupItem::ATPickupItem()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	if (SphereComp)
	{
		SetRootComponent(SphereComp);
		SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Overlap);
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (MeshComp)
	{
		MeshComp->SetupAttachment(GetRootComponent());
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

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
	if (OtherActor == nullptr) return;

	auto PlayerTank = Cast<ATPlayerTank>(OtherActor);
	if (PlayerTank)
	{
		
		// TODO: Only add health to player if not at max health
		float PlayerCurrentHealth = PlayerTank->GetHealth();
		if (PlayerCurrentHealth >= 99.9f)
		{
			return;
		}
		else
		{ 
			PlayerTank->GiveHealth(100.f);
			Destroy();
		}
	}
}


