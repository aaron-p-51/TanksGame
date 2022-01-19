// Fill out your copyright notice in the Description page of Project Settings.


#include "THealthComponent.h"


// Engine Includes
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Game Includes
#include "TDisplayCausedDamageInterface.h"


// Sets default values for this component's properties
UTHealthComponent::UTHealthComponent()
{
	DefaultHealth = 100;
	Health = DefaultHealth;
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
	}

	bCanBeDamaged = true;
	bIsInvincible = false;
}


void UTHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage > 0 && bCanBeDamaged && !bIsInvincible)
	{
		// Check for self damage. Self damage is not allowed
		if (InstigatedBy)
		{
			APawn* InstigatorPawn = InstigatedBy->GetPawn();
			if (InstigatorPawn && InstigatorPawn == DamagedActor)
			{
				return;
			}
		}
		
		// Do not do more damage than Health
		float DamageAmount = (Damage > Health) ? Health + 1 : Damage;

		AlertDamageToDisplayCausedDamageInterfaceController(InstigatedBy, DamagedActor, DamageAmount);

		// Clamp Health so it stays between starting health and 0
		Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

		OnHealthChange.Broadcast(Health, -DamageAmount, DamageType, InstigatedBy, DamageCauser);
	}	
}


void UTHealthComponent::AlertDamageToDisplayCausedDamageInterfaceController(AController* DamageCausingController, AActor* DamagedActor, float DamageAmount) const
{
	if (DamageCausingController)
	{
		ITDisplayCausedDamageInterface* DisplayDamageInterface = Cast<ITDisplayCausedDamageInterface>(DamageCausingController);
		if (DisplayDamageInterface)
		{
			DisplayDamageInterface->DisplayDamageCaused(DamagedActor, DamageAmount);
		}
	}
}


void UTHealthComponent::OnRep_Health()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Health"));
	OnHealthChange.Broadcast(Health, 0, nullptr, nullptr, nullptr);
}


float UTHealthComponent::AddHealth(float Amount)
{
	if (Amount > 0.f && GetOwnerRole() == ENetRole::ROLE_Authority && Health < DefaultHealth)
	{
		// Only Heal up to default health
		float HealAmount = (Health + Amount > DefaultHealth) ? (DefaultHealth - Health) : Amount;

		// New Health can not be more than DefaultHealth (should be equivalent to Health + HealAmount)
		Health = FMath::Min(Health + Amount, DefaultHealth);

		OnHealthChange.Broadcast(Health, HealAmount, nullptr, nullptr, nullptr);

		return HealAmount;
	}

	return 0.f;
}


void UTHealthComponent::SetCanBeDamaged(bool CanBeDamaged)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		bCanBeDamaged = CanBeDamaged;
	}
}


void UTHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION(UTHealthComponent, Health, COND_OwnerOnly);
}

