// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TSoldierAIController.h"

// Engine Includes
#include "Kismet/KismetMathLibrary.h"

// Game Includes
#include "TShootableWeaponOwner.h"

void ATSoldierAIController::FireAtTarget(float Duration, float DurationVariance)
{
	if (WithinMaxAngleToTarget())
	{
		Super::FireAtTarget(Duration, DurationVariance);
	}
}
