// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TAirDropItem.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATAirDropItem::ATAirDropItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	if (CollisionComponent)
	{
		SetRootComponent(CollisionComponent);
		CollisionComponent->SetMobility(EComponentMobility::Movable);
	}

	FallSpeed = 1000.f;
	bShouldDisappear = false;
	ScaleRateChange = 1.f;
	SetReplicates(true);
	SetReplicateMovement(false);
}


// Called every frame
void ATAirDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldDisappear)
	{	
		// Fall to drop Zone
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, FallSpeed));
	}
	else
	{
		float SizeChangeThisFrame = DeltaTime * ScaleRateChange;
		SetActorScale3D(GetActorScale3D() - SizeChangeThisFrame);

		if (GetActorScale3D().X < 0.f)
		{
			Destroy();
		}
	}
}


void ATAirDropItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATAirDropItem, TargetLocation);
	DOREPLIFETIME(ATAirDropItem, bShouldDisappear);
}

