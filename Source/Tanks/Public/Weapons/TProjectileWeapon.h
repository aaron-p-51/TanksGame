// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TShootableWeapon.h"
#include "TProjectileWeapon.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;


/**
 * 
 */
UCLASS()
class TANKS_API ATProjectileWeapon : public ATShootableWeapon
{

	GENERATED_BODY()

/*
* Members *
*/
protected:

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Porjectile")
	TSubclassOf<class ATProjectile> ProjectileClass;

	/** Forward offset from calculated spawn position to spawn projectile */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
	float ProjectileForwardOffsetSpawn;

	/** Projectile launch speed set at spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float ProjectileLaunchSpeed;

/*
* Methods
*/

public:

	ATProjectileWeapon();


protected:

	virtual void BeginPlay() override;

	/** [Server] Fire a new projectile */
	virtual void FireWeapon() override;

	UFUNCTION(Server, Reliable)
	void ServerFireWeapon();

	/**
	 * [Server] Get the fire point and rotation for where to spawn weapon projectile
	 * 
	 * @param FireLocation		Will be set to calculated projectile spawn location
	 * @param FireRotation		Will be set to calculated projectile spawn rotation
	 */
	bool GetFirePositionAndRotation(FVector& FireLocation, FRotator& FireRotation) const;


	/**
	 * [Server] Spawn a projectile of type @ProjectileClass. Will spawn at Location and Rotation
	 * ProjectileForwardOffsetSpawn will be applied to spawn transform created from Location and 
	 * Rotation parameters
	 */
	void SpawnProjectile(const FVector& Location, const FRotator& Rotation);

};
