// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TBoostComponent.h"


#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTBoostComponent::UTBoostComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Boost = 100.f;
	MaxBoost = 100.f;
	UseRate = 5.f;
	RechargeRate = 8.f;
	RechargeDelay = 4.f;
	BoostState = EBoostState::EBS_Charged;
}


void UTBoostComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only server should update boost level, boost level is replicated to clients
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		UpdateBoostLevel(DeltaTime);
	}	
}


void UTBoostComponent::UpdateBoostLevel(float DeltaTime)
{
	// Store value of Boost before updating. Only fire OnBoostChange event
	// when boost changes.
	float PreUpdateBoost = Boost;

	// Deplete boost
	if (BoostState == EBoostState::EBS_Using)
	{
		if (Boost > 0.f)
		{
			Boost = Boost - UseRate * DeltaTime;
		}
	}
	// Recharge boost
	else if (BoostState == EBoostState::EBA_Recharging)
	{
		if (Boost < MaxBoost)
		{
			Boost = Boost + RechargeRate * DeltaTime;
		}
	}

	if (PreUpdateBoost != Boost)
	{
		OnRep_Boost();
	}
}


void UTBoostComponent::StartUseBoost() 
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerStartUseBoost();
		return;
	}

	if (Boost > 0.f)
	{
		BoostState = EBoostState::EBS_Using;
	}
}


void UTBoostComponent::ServerStartUseBoost_Implementation()
{
	StartUseBoost();
}


void UTBoostComponent::StopUseBoost()
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerStopUseBoost();
		return;
	}

	if (Boost < MaxBoost)
	{
		// if no recharge delay BoostState is set to Recharging immediately
		if (RechargeDelay > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(RechargeDelayTimerHandle, this, &UTBoostComponent::RechargeDelayComplete, RechargeDelay, false);
		}
		else
		{
			RechargeDelayComplete();
		}
		BoostState = EBoostState::EBA_Recharging;
	}	
}


void UTBoostComponent::ServerStopUseBoost_Implementation()
{
	StopUseBoost();
}


void UTBoostComponent::RechargeDelayComplete()
{
	BoostState = EBoostState::EBA_Recharging;
}


void UTBoostComponent::OnRep_Boost()
{
	bool UsingBoost = BoostState == EBoostState::EBS_Using;
	OnBoostChange.Broadcast(Boost, UsingBoost);
}


void UTBoostComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTBoostComponent, Boost);
}


