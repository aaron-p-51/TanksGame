// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TAIFireProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "StaticLibs/TBallisticsCalculator.h"
#include "Weapons/TProjectile.h"


const static int32 LOW_ANGLE_SOLUTION = 0;
const static int32 HIGH_ANGLE_SOLUTION = 1;


UTAIFireProjectile::UTAIFireProjectile()
{
	BallisticMode = EBallisticMode::EBM_NormalBallisticArc;
	ProjectileVelocity = 2000.f;
	ProjectileGravity = 980.f;
	bUseRandomTargetOffset = true;
	RandomTargetOffsetRadius = 250.f;
	PerferredBallisticArc = EPerferredBallisticArc::EPBA_LowAngleSolution;
	ArcPeak = 100.f;
	MinVerticalLaunchAngle = 0.f;
	MaxVerticalLaunchAngle = 90.f;
}


void UTAIFireProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Get grativy from world settings. If EBallisticMode == EBM_FixedLateralSpeedCustomArc
	// projectile gravity will be scaled from world settings gravity
	auto WorldSettings = GetWorld()->GetWorldSettings();
	if (WorldSettings)
	{
		WorldGravity = FMath::Abs(GetWorld()->GetWorldSettings()->GetGravityZ());
	}
	else
	{
		WorldGravity = 980.f;
	}

}


void UTAIFireProjectile::FireProjectile(FVector FirePointLocation, AActor* Target)
{
	
	if (!Target) return;
	FVector TargetLocation = Target->GetActorLocation();
	FVector TargetVelocity = Target->GetVelocity();

	if (BallisticMode == EBallisticMode::EBM_NormalBallisticArc)
	{
		FireProjectileNormalArc(FirePointLocation, TargetLocation, TargetVelocity);
	}
	else /* BallisticMode == EBM_FixedLateralSpeedCustomArc */
	{
		FireProjectileCustomArc(FirePointLocation, TargetLocation, TargetVelocity);
	}
}


void UTAIFireProjectile::FireProjectileNormalArc(FVector FirePointLocation, FVector TargetLocation, FVector TargetVelocity)
{
	if (bUseRandomTargetOffset && RandomTargetOffsetRadius > 0.f)
	{
		ApplyRandomTargetOffset(TargetLocation);
	}

	FBallisticArcSolution BallisticArcSolution;

	int32 ValidSolutions = 0;

	if (TargetVelocity.SizeSquared() > 0)
	{
		ValidSolutions = UTBallisticsCalculator::SolveBallisticArcMovingTarget(FirePointLocation, ProjectileVelocity, TargetLocation, TargetVelocity, ProjectileGravity, BallisticArcSolution);
	}
	else
	{
		ValidSolutions = UTBallisticsCalculator::SolveBallisticArc(FirePointLocation, ProjectileVelocity, TargetLocation, ProjectileGravity, BallisticArcSolution);
	}

	if (ValidSolutions != 0)
	{
		bool bLowAngleValid = BallisticArcSolution.LowAngleFireVelocity != FVector::ZeroVector;
		bool bHighAngleValid = BallisticArcSolution.HighAngleFireVelocity != FVector::ZeroVector;
		ESelectedBallisticArc SelectedSolution = SelectDesiredNormalArcSolution(ValidSolutions, bLowAngleValid, bHighAngleValid);

		if (SelectedSolution == ESelectedBallisticArc::ESBA_LowAngleSolution && 
			IsWithinLaunchAngle(FirePointLocation, BallisticArcSolution.LowAngleFireVelocity))
		{
			MulticastSpawnAndLaunchProjectile(FirePointLocation, BallisticArcSolution.LowAngleFireVelocity, WorldGravity);
		}
		else if (SelectedSolution == ESelectedBallisticArc::ESBA_HighAngleSolution && 
			IsWithinLaunchAngle(FirePointLocation, BallisticArcSolution.HighAngleFireVelocity))
		{
			MulticastSpawnAndLaunchProjectile(FirePointLocation, BallisticArcSolution.HighAngleFireVelocity, WorldGravity);
		}
	}
}


ESelectedBallisticArc UTAIFireProjectile::SelectDesiredNormalArcSolution(int32 ValidSolutions, bool bLowAngleSolutionValid, bool bHighAngleSolutionValid)
{
	switch (PerferredBallisticArc)
	{
	case EPerferredBallisticArc::EPBA_AnySolution:
		if (ValidSolutions == 1)
		{
			if (bLowAngleSolutionValid)
			{
				return ESelectedBallisticArc::ESBA_LowAngleSolution;
			}
			else
			{
				return ESelectedBallisticArc::ESBA_HighAngleSolution;
			}
		}
		else if (ValidSolutions == 2)
		{
			int RamdomSolution = FMath::RandRange(0, 1);
			if (RamdomSolution == 0)
			{
				return ESelectedBallisticArc::ESBA_LowAngleSolution;
			}
			else
			{
				return ESelectedBallisticArc::ESBA_HighAngleSolution;
			}
		}
		break;

	case EPerferredBallisticArc::EPBA_AnySolutionHighBias:
		if (bHighAngleSolutionValid)
		{
			return ESelectedBallisticArc::ESBA_HighAngleSolution;
		}
		else if (bLowAngleSolutionValid)
		{
			return ESelectedBallisticArc::ESBA_LowAngleSolution;
		}
		break;

	case EPerferredBallisticArc::EPBA_AnySolutionLowBias:
		if (bLowAngleSolutionValid)
		{
			return ESelectedBallisticArc::ESBA_LowAngleSolution;
		}
		else if (bHighAngleSolutionValid)
		{
			return ESelectedBallisticArc::ESBA_HighAngleSolution;
		}
		break;

	case EPerferredBallisticArc::EPBA_HighAngleSolution:
		if (bHighAngleSolutionValid)
		{
			return ESelectedBallisticArc::ESBA_HighAngleSolution;
		}
		break;

	case EPerferredBallisticArc::EPBA_LowAngleSolution:
		if (bLowAngleSolutionValid)
		{
			return ESelectedBallisticArc::ESBA_LowAngleSolution;
		}
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unspecified EPerferredBallisticArc"));
		break;
	}

	return ESelectedBallisticArc::ESBA_NoSolutionSelected;
}


void UTAIFireProjectile::MulticastSpawnAndLaunchProjectile_Implementation(FVector SpawnLocation, FVector LaunchVelocity, float Gravity)
{
	FTransform ProjectileSpawnTransform = FTransform(FRotator(0, 0, 0), SpawnLocation);
	ESpawnActorCollisionHandlingMethod SpawnActorCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATProjectile* Projectile = Cast<ATProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ProjectileClass, ProjectileSpawnTransform, SpawnActorCollisionHandlingMethod));
	if (Projectile)
	{
		float GravityScale = Gravity / WorldGravity;
		Projectile->SetLaunchVelocityVector(LaunchVelocity, GravityScale);

		UGameplayStatics::FinishSpawningActor(Projectile, ProjectileSpawnTransform);
	}
}


void UTAIFireProjectile::FireProjectileCustomArc(FVector FirePointLocation, FVector TargetLocation, FVector TargetVelocity)
{
	if (bUseRandomTargetOffset && RandomTargetOffsetRadius > 0.f)
	{
		ApplyRandomTargetOffset(TargetLocation);
	}


	if (TargetVelocity.SizeSquared() > 0)
	{
		FBallisticArcLateralMovingTargetSolution BallisticArcLateralMovingTargetSolution;
		UTBallisticsCalculator::SolveBallisticArcLateralMovingTarget(FirePointLocation, ProjectileVelocity, TargetLocation, TargetVelocity, ArcPeak, BallisticArcLateralMovingTargetSolution);
		
		if (IsWithinLaunchAngle(FirePointLocation, BallisticArcLateralMovingTargetSolution.FireVelocity))
		{
			MulticastSpawnAndLaunchProjectile(FirePointLocation, BallisticArcLateralMovingTargetSolution.FireVelocity, BallisticArcLateralMovingTargetSolution.Gravity);
		}
	}
	else
	{
		FBallisticArcLateralSolution BallisticArcLateralSolution;
		UTBallisticsCalculator::SolveBallisticArcLateral(FirePointLocation, ProjectileVelocity, TargetLocation, ArcPeak, BallisticArcLateralSolution);

		if (IsWithinLaunchAngle(FirePointLocation, BallisticArcLateralSolution.FireVelocity))
		{
			MulticastSpawnAndLaunchProjectile(FirePointLocation, BallisticArcLateralSolution.FireVelocity, BallisticArcLateralSolution.Gravity);
		}
	}
}


void UTAIFireProjectile::ApplyRandomTargetOffset(FVector& TargetPosition)
{
	FVector RandomGroundVector = FVector(FMath::VRand());
	RandomGroundVector *= RandomTargetOffsetRadius;
	TargetPosition += RandomGroundVector;
}


bool UTAIFireProjectile::IsWithinLaunchAngle(const FVector& LaunchPoint, const FVector& LaunchVelocity) const
{
	// TODO: Add check for launch rotation, left and right of turret

	FRotator LaunchRotation = LaunchVelocity.Rotation();
	//return LaunchRotation.Pitch >= MinVerticalLaunchAngle && LaunchRotation.Pitch <= MaxVerticalLaunchAngle;
	return true;
}
