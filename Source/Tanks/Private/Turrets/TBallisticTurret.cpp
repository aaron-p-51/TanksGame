// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/TBallisticTurret.h"

// Game Includes
#include "Components/TAIFireProjectile.h"


ATBallisticTurret::ATBallisticTurret()
{
	FireProjectileComp = CreateDefaultSubobject<UTAIFireProjectile>(TEXT("AIFireProjectile"));
}


void ATBallisticTurret::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_FireTurret, this, &ATBallisticTurret::FireTurret, GetNextFireTime());
}


void ATBallisticTurret::FireTurret()
{
	if (FireProjectileComp)
	{
		APawn* Target = LookForHostileTarget();
		if (Target)
		{
			FireProjectileComp->FireProjectile(FirePoint->GetComponentLocation(), Target);
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_FireTurret, this, &ATBallisticTurret::FireTurret, GetNextFireTime());
}


float ATBallisticTurret::GetNextFireTime()
{
	float NextFireTime = FMath::RandRange(TimeBetweenFires - TimeBetweenFiresDeviation, TimeBetweenFires + TimeBetweenFiresDeviation);
	if (NextFireTime < 0)
	{
		NextFireTime = FMath::RandRange(TimeBetweenFires, TimeBetweenFires + TimeBetweenFiresDeviation);
	}

	return NextFireTime;
}

