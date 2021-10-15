// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TItemDropZone.h"

// Engine Includes
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Game Includes
#include "Items/TAirDropItem.h"

//// Sets default values
ATItemDropZone::ATItemDropZone()
{
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	if (BoxCollisionComponent)
	{
		SetRootComponent(BoxCollisionComponent);

		// We only want DropZone to collide with ATAirDropItem
		BoxCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoxCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		BoxCollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

		BoxCollisionComponent->SetBoxExtent(FVector(100.f, 100.f, 32.f));
	}

	ItemPickupSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemPickupSpawnPoint"));
	if (ItemPickupSpawnPoint)
	{
		ItemPickupSpawnPoint->SetupAttachment(GetRootComponent());
	}

	CurrentPickupItemInDropZone = nullptr;
}


// Called when the game starts or when spawned
void ATItemDropZone::BeginPlay()
{
	Super::BeginPlay();

	if (BoxCollisionComponent && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATItemDropZone::OnBoxCollisionComponentBeginOverlap);
	}
}


EItemDropZoneState ATItemDropZone::GetItemDropZoneState()
{
	// Update state if item has been picked up
	if (ItemDropZoneState == EItemDropZoneState::EIDZS_ItemInDropZone)
	{
		if (CurrentPickupItemInDropZone == nullptr)
		{
			return EItemDropZoneState::EIDZS_Empty;
		}
		else
		{
			return EItemDropZoneState::EIDZS_ItemInDropZone;
		}
	}
	else
	{
		return ItemDropZoneState;
	}
}


void ATItemDropZone::SetItemDropZoneState(EItemDropZoneState NewState)
{
	ItemDropZoneState = NewState;
}


void ATItemDropZone::OnBoxCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	auto AirDropItem = Cast<ATAirDropItem>(OtherActor);
	if (AirDropItem)
	{
		AirDropItem->bShouldDisappear = true;

		if (PickupIcon)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(GetActorLocation());

			// Apply Spawn Location offset
			if (ItemPickupSpawnPoint)
			{
				SpawnTransform.SetLocation(SpawnTransform.GetLocation() + ItemPickupSpawnPoint->GetRelativeLocation());
			}

			AActor* ItemToSpawn = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), PickupIcon, SpawnTransform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (ItemToSpawn)
			{
				CurrentPickupItemInDropZone = ItemToSpawn;
				SetItemDropZoneState(EItemDropZoneState::EIDZS_ItemInDropZone);

				UGameplayStatics::FinishSpawningActor(ItemToSpawn, SpawnTransform);
			}
		}
	}
}



