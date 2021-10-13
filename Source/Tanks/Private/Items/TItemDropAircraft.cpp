// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TItemDropAircraft.h"

// Engine Includes
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/AudioComponent.h"

// Game Includes
#include "Items/TItemDropZone.h"
#include "Items/TAirDropItem.h"

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

	Speed = 100.f;
	MaxTravelDistance = 3000.f;
	DistanceToDropItem = 1000.f;
	CurrentTravelDistance = 0.f;
	bItemDropped = false;

	SetReplicates(true);
	SetReplicateMovement(false);
}


void ATItemDropAircraft::SetFlightPathParams(float TravelDistance, FVector DropZoneLocation)
{
	ItemDropZoneLocation = DropZoneLocation;

	// Set Drop point directly above DropZoneLocation
	FVector AirDropLocation = ItemDropZoneLocation;
	AirDropLocation.Z = GetActorLocation().Z;

	DistanceToDropItem = (AirDropLocation - GetActorLocation()).Size();
	if (TravelDistance > DistanceToDropItem)
	{
		MaxTravelDistance = TravelDistance;
	}
	else
	{
		MaxTravelDistance = DistanceToDropItem * 2.f;
	}
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

	CheckToDropItem();

	UpdateAircraftLocation(DeltaTime);

	if (GetLocalRole() == ENetRole::ROLE_Authority && CurrentTravelDistance > MaxTravelDistance)
	{
		Destroy();
	}
}


void ATItemDropAircraft::CheckToDropItem()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (!bItemDropped && CurrentTravelDistance >= DistanceToDropItem)
		{
			if (DropItem())
			{
				bItemDropped = true;
			}
		}
	}
}


void ATItemDropAircraft::UpdateAircraftLocation(float DeltaTime)
{
	float DistanceDelta = Speed * DeltaTime;
	CurrentTravelDistance += DistanceDelta;
	FVector NewPosition = GetActorLocation() + (GetActorForwardVector() * DistanceDelta);
	SetActorLocation(NewPosition);
}


bool ATItemDropAircraft::DropItem()
{
	FTransform ItemDropTransform;
	ItemDropTransform.SetLocation(GetActorLocation());
	if (ItemToDropClass)
	{
		ATAirDropItem* Item = Cast<ATAirDropItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ItemToDropClass, ItemDropTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

		if (Item)
		{
			Item->TargetLocation = ItemDropZoneLocation;
			UGameplayStatics::FinishSpawningActor(Item, ItemDropTransform);
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item"));
	}

	return false;
}




void ATItemDropAircraft::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATItemDropAircraft, MaxTravelDistance);
	DOREPLIFETIME(ATItemDropAircraft, ItemDropZoneLocation);
	DOREPLIFETIME(ATItemDropAircraft, bItemDropped);
}

