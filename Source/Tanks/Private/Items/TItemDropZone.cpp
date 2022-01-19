// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/TItemDropZone.h"


// Sets default values
ATItemDropZone::ATItemDropZone()
{
	ItemPickupSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemPickupSpawnPoint"));
	ItemPickupSpawnPoint->SetupAttachment(GetRootComponent());
	

	CurrentPickupItemInDropZone = nullptr;
}


void ATItemDropZone::SetCurrentPickupItemInDropZone(AActor* Actor)
{
#if WITH_EDITOR

	if (CurrentPickupItemInDropZone)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Attempting to set CurrentPickupItemInDropZone. Already valid"), *GetName());
	}

#endif

	CurrentPickupItemInDropZone = Actor;
}

void ATItemDropZone::SetItemDropZoneState(EItemDropZoneState NewState)
{
	
#if WITH_EDITOR

	if (NewState == EItemDropZoneState::EIDZS_Empty && CurrentPickupItemInDropZone)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Attempting to set state to EIDZS_Empty but CurrentPickupItemInDropZone is still valid!"), *GetName());
	}
	else if (NewState == EItemDropZoneState::EIDZS_ItemInbound && CurrentPickupItemInDropZone)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Attempting to set state to EIDZS_ItemInbound but CurrentPickupItemInDropZone is still valid!"), *GetName());
	}
	else if (NewState == EItemDropZoneState::EIDZS_ItemInDropZone && !CurrentPickupItemInDropZone)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Attempting to set state to EIDZS_ItemInDropZone but CurrentPickupItemInDropZone is not valid!"), *GetName());
	}

#endif

	ItemDropZoneState = NewState;
}


FVector ATItemDropZone::GetItemPickupSpawnPoint() const
{
	if (ItemPickupSpawnPoint)
	{
		return ItemPickupSpawnPoint->GetComponentLocation();
	}

	return GetActorLocation();
}

