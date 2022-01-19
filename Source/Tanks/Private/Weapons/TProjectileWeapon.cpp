// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TProjectileWeapon.h"

// Engine Includes
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Game Includes
#include "Pawns/TPlayerTank.h"
#include "Weapons/TProjectile.h"
#include "TShootableWeaponOwner.h"

ATProjectileWeapon::ATProjectileWeapon()
{
	ProjectileLaunchSpeed = 1000.f;

	SetReplicates(true);
}


void ATProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	ShootableWeaponType = EShootableWeaponType::ESWT_Projectile;
}


void ATProjectileWeapon::FireWeapon()
{
	// Only spawn projectile on server. Projectiles are set to replicate
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerFireWeapon();
		return;
	}

	FVector FireLocation;
	FRotator FireRotation;
	
	if (GetFirePositionAndRotation(FireLocation, FireRotation))
	{		
		SpawnProjectile(FireLocation, FireRotation);
	}
}


void ATProjectileWeapon::ServerFireWeapon_Implementation()
{
	FireWeapon();
}


bool ATProjectileWeapon::GetFirePositionAndRotation(FVector& FireLocation, FRotator& FireRotation) const
{
	// Default behavior and intended use is for owner of this weapon to implement
	// ITShootableWeaponOwner interface. ITShootableWeaponOwner alows for
	// owner to supply weapon fire location and position
	ITShootableWeaponOwner* OwnerInterface = Cast<ITShootableWeaponOwner>(GetOwner());

	if (bOwnerSetsFirePoint && OwnerInterface)
	{
		OwnerInterface->GetShootableWeaponFirePoint(FireLocation, FireRotation);
		return true;
	}
	else if (FirePoint)
	{
		FireLocation = FirePoint->GetComponentLocation();
		FireRotation = FirePoint->GetComponentRotation();
		return true;
	}

	return false;
}


void ATProjectileWeapon::SpawnProjectile(const FVector& Location, const FRotator& Rotation)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return;

	FTransform ProjectileSpawnTransform(Rotation.Quaternion(), Location + (Rotation.Vector() * ProjectileForwardOffsetSpawn));
	ATProjectile* Projectile = Cast<ATProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, ProjectileSpawnTransform));
	if (Projectile)
	{
		Projectile->SetOwner(this);

		if (MyPawn)
		{
			Projectile->SetInstigator(MyPawn);
		}

		Projectile->SetDamageAmountInExplosion(DamageAmount);
		Projectile->SetDamageType(DamageType);
		Projectile->SetLaunchSpeed(ProjectileLaunchSpeed);
		UGameplayStatics::FinishSpawningActor(Projectile, ProjectileSpawnTransform);
	}
}

