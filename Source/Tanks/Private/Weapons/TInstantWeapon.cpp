// Fill out your copyright notice in the Description page of Project Settings.

// Engine Includes
#include "Weapons/TInstantWeapon.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "AIController.h"



// Game Includes
#include "Pawns/TPlayerTank.h"
#include "Tanks/Tanks.h"
#include "AI/TTankAIVehicleController.h"
#include "AI/TBaseAIController.h"
#include "TShootableWeaponOwner.h"

const static FName AUDIO_SURFACE_SELECTOR = TEXT("AudioSurfaceSelector");

const static int32 AUDIO_SURFACE_DEFAULT = 0;
const static int32 AUDIO_SURFACE_DEFAULT_METAL = 1;


ATInstantWeapon::ATInstantWeapon()
{

	LineTraceDistance = 10000.0f;
	SetReplicates(true);

	ShootableWeaponType = EShootableWeaponType::ESWT_Instant;
}


void ATInstantWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void ATInstantWeapon::FireWeapon()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerFire();
	}

	FVector LineTraceStart, LineTraceEnd;
	SetLineTraceEndPoints(LineTraceStart, LineTraceEnd);

	FHitResult Hit;
	if (WeaponFireLineTrace(Hit, LineTraceStart, LineTraceEnd))
	{
		ProcessWeaponLineTraceHit(Hit, LineTraceStart, LineTraceEnd);
	}
}


void ATInstantWeapon::ServerFire_Implementation()
{
	FireWeapon();
}


void ATInstantWeapon::SetLineTraceEndPoints(FVector& LineTraceStart, FVector& LineTraceEnd) const
{
	FVector ShotDirection = FVector::ZeroVector;
	FRotator LineTraceRotation = FRotator::ZeroRotator;
	ITShootableWeaponOwner* OwnerInterface = Cast<ITShootableWeaponOwner>(MyPawn);

	// ITShootableWeaponOwner defines where trace should be originate from.
	if (bOwnerSetsFirePoint && OwnerInterface)
	{
		OwnerInterface->GetShootableWeaponFirePoint(LineTraceStart, LineTraceRotation);
	}
	else if (FirePoint)
	{
		LineTraceStart = FirePoint->GetComponentLocation();
		LineTraceRotation = FirePoint->GetComponentRotation();
	}
	else
	{
		return;
	}

	FVector LineTraceDirection = LineTraceRotation.Vector();

	if (bUseBulletSpread)
	{
		LineTraceDirection = ApplyBulletSpread(LineTraceDirection);
	}

	LineTraceEnd = LineTraceStart + (LineTraceDirection * LineTraceDistance);
}



bool ATInstantWeapon::WeaponFireLineTrace(FHitResult& HitResult, const FVector& LineTraceStart, const FVector& LineTraceEnd) const
{
	FCollisionQueryParams CollisionQueryParams;
	if (MyPawn)
	{
		CollisionQueryParams.AddIgnoredActor(MyPawn);
	}
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.bReturnPhysicalMaterial = true;
	

	bool result = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, ECC_Visibility, CollisionQueryParams);

	bool temp = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get()) == EPhysicalSurface::SurfaceType2;
	if (temp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Grass"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit %d"), UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get()));
	return result;
}


FVector ATInstantWeapon::ApplyBulletSpread(const FVector& ShotDirection) const
{
	// Bullet spread should be small (ie less than 3) see @BulletSpread
	float HalfRad = FMath::DegreesToRadians(BulletSpread);
	return FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
}


void ATInstantWeapon::ProcessWeaponLineTraceHit(const FHitResult& Hit, const FVector& Origin, const FVector& Direction)
{
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
	
	PlayImpactEffects(HitSurfaceType, Hit.ImpactPoint, Direction);

	// When called on server will apply damage and replicate HitScanTrace used for FX on non owning clients
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), DamageAmount, Direction, Hit, MyPawn->GetController(), this, DamageType);
		HitScanTrace.SetHitScanTraceValues(HitSurfaceType, Hit.ImpactPoint, Direction);
	}
}


void ATInstantWeapon::PlayImpactEffects(TEnumAsByte<EPhysicalSurface> Surface, const FVector& ImpactPoint, const FVector& ImpactDirection) const
{
	const FImpactEffects* ImpactEffect = ImpactEffects.Find(Surface);
	if (ImpactEffect)
	{
		// Play SFX
		if (ImpactEffect->ImpactSoundCue)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactEffect->ImpactSoundCue, ImpactPoint);
		}

		// Play VFX it is assumed either ParticleEffect, or NiagaraEffect will be null
		if (ImpactEffect->ParticleEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect->ParticleEffect, ImpactPoint, FRotator::MakeFromEuler(ImpactDirection * -1.f));
		}

		if (ImpactEffect->NiagaraEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect->NiagaraEffect, ImpactPoint, FRotator::MakeFromEuler(ImpactDirection * -1.f));
		}
	}
	else // use default FX
	{
		if (DefaultImpactSoundCue)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DefaultImpactSoundCue, ImpactPoint);
		}

		if (DefaultImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactParticles, ImpactPoint, FRotator::MakeFromEuler(ImpactDirection * -1.f));
		}
	}
}


void ATInstantWeapon::OnRep_HitScanTrace()
{
	PlayImpactEffects(HitScanTrace.HitSurfaceType, HitScanTrace.HitLocation, HitScanTrace.HitDirection);
}


void ATInstantWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATInstantWeapon, HitScanTrace, COND_SkipOwner);
}

