// Fill out your copyright notice in the Description page of Project Settings.


#include "THealthComponent.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "Player/TBasePlayerController.h"
#include "TDisplayCausedDamageInterface.h"
#include "GameModes/TBaseGameMode.h"

// Sets default values for this component's properties
UTHealthComponent::UTHealthComponent()
{
	DefaultHealth = 100;
}


// Called when the game starts
void UTHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UTHealthComponent::HandleTakeAnyDamage);
		}

		UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
		if (GI)
		{
			UTMWGlobalEventHandler* EventHandlerSubsystem = GI->GetSubsystem<UTMWGlobalEventHandler>();
			if (EventHandlerSubsystem)
			{
				BoundGameStateChangedEvent.BindDynamic(this, &UTHealthComponent::OnGameStateEventReceived);
				EventHandlerSubsystem->BindGlobalEventByClass(UTEventGameStateChange::StaticClass(), BoundGameStateChangedEvent);
			}
		}

	}

	Health = DefaultHealth;


}


void UTHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0 || !bCanBeDamaged || bIsInvincible) return;

	// Do not do more damage than Health
	float DamageAmount = (Damage > Health) ? Health + 1 : Damage;

	if (InstigatedBy)
	{
		auto DisplayDamageInterface = Cast<ITDisplayCausedDamageInterface>(InstigatedBy);
		if (DisplayDamageInterface)
		{
			DisplayDamageInterface->DisplayDamageCaused(DamagedActor, DamageAmount);
		}

		// Do not damage yourself
		auto InstigatorPawn = InstigatedBy->GetPawn();
		if (InstigatorPawn && InstigatorPawn == DamagedActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to damage yourself, damage pervented"));
			return;
		}
	}
	
	
	// Clamp Health so it stays between starting health and 0
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	OnHealthChange.Broadcast(this, Health, -DamageAmount, DamageType, InstigatedBy, DamageCauser);
}


void UTHealthComponent::AddHealth(float Amount)
{
	if (Amount <= 0 || Health <= 0.0f || Health >= DefaultHealth) return;
	
	// Only Heal up to default health
	float HealAmount = (Health + Amount > DefaultHealth) ? (DefaultHealth - Health) : Amount;

	// New Health can not be more than DefaultHealth (should be equivalent to Health + HealAmount)
	Health = FMath::Min(Health + Amount, DefaultHealth);

	OnHealthChange.Broadcast(this, Health, HealAmount, nullptr, nullptr, nullptr);
}


void UTHealthComponent::AddApplyDamageInGameState(EGameInProgressState State)
{
	if (!ApplyDamageInGameStates.Contains(State))
	{
		ApplyDamageInGameStates.Add(State);
	}
}


void UTHealthComponent::OnGameStateEventReceived(UObject* Publisher, UObject* Payload, const TArray<FString>& Metadata)
{
	if (Payload)
	{
		UTEventGameStateChange* CurrentGameState = Cast<UTEventGameStateChange>(Payload);
		if (CurrentGameState)
		{
			bCanBeDamaged = ApplyDamageInGameStates.Contains(CurrentGameState->NewGameState);
		}
	}
}

