// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TBaseAIController.h"

// Engine Includes
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"

// Game Includes
#include "TShootableWeaponOwner.h"



ATBaseAIController::ATBaseAIController(const FObjectInitializer& ObjectInitializer)
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPercenptionComp");


	TeamNumber = 35;
	TeamID = FGenericTeamId(TeamNumber);
}

ETeamAttitude::Type ATBaseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
			if (OtherTeamID == TeamNumber)
			{
				return ETeamAttitude::Friendly;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}
	}

	return ETeamAttitude::Neutral;
}


void ATBaseAIController::FireAtTarget(float Duration, float DurationVariance)
{
	
	auto WeaponOwner = Cast<ITShootableWeaponOwner>(GetPawn());
	if (WeaponOwner)
	{
		if (WeaponOwner->IsFiring_Implementation()) return;
		if (GetWorldTimerManager().IsTimerActive(TimerHandle_FireTime)) return;

		float TotalFireTime = Duration + UKismetMathLibrary::RandomFloatInRange(-DurationVariance, DurationVariance);

		if (TotalFireTime <= 0) return;
		WeaponOwner->StartFire_Implementation();
		GetWorldTimerManager().SetTimer(TimerHandle_FireTime, this, &ATBaseAIController::StopFireAtTarget, TotalFireTime);
		
	}
}

void ATBaseAIController::StopFireAtTarget()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_FireTime);

	auto WeaponOwner = Cast<ITShootableWeaponOwner>(GetPawn());
	if (WeaponOwner)
	{
		WeaponOwner->StopFire_Implementation();
		
	}
}


bool ATBaseAIController::WithinMaxAngleToTarget()
{
	if (CurrentCombatTarget && GetPawn())
	{
		float YawLookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), CurrentCombatTarget->GetActorLocation()).Yaw;
		float AngleToTarget = YawLookAtRotation - GetPawn()->GetActorRotation().Yaw;
		if (AngleToTarget > 180.f)
		{
			AngleToTarget -= 360.f;
		}
		else if (AngleToTarget < -180.f)
		{
			AngleToTarget += 360.f;
		}

		return (FMath::Abs(AngleToTarget) <= MaxAngleToFireAtTarget) ? true : false;
	}

	return false;
}


