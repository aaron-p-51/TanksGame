// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class USoundBase;
class UAudioComponent;

UCLASS()
class TANKS_API ATProjectile : public AActor
{
	GENERATED_BODY()

/*
* Members *
*/

	/**************************************************************************/
	/* Components */
	/**************************************************************************/
protected:

	/** Collision Comp for Projectile, ApplyRadialDamage will be called when collision detected */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components");
	USphereComponent* CollisionComp;

	/** Mesh for projectile */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	/** Projectile Movement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComp;

	/** VFX for projectile trail, will deactivate when collision detected */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components | VFX")
	UParticleSystemComponent* TrailEffectComp;


	/**************************************************************************/
	/* FX */
	/**************************************************************************/
protected:

	/** VFX when Collision detected */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | VFX")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | SFX")
	UAudioComponent* ProjectileSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | SFX")
	USoundBase* ProjectileImpactSFX;


	/**************************************************************************/
	/* Damage */
	/**************************************************************************/
protected:

	/** Type of damage done in ApplyRadialDamage upon collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	/** Amount of damage the projectile does, should be set by Instigator Weapon (TProjectileWeapon) using SetDamageAmountInExplosion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float DamageAmount;

	/** Damage Radius from impact point, should be set by Instigator Weapon (TProjectileWeapon) using SetDamageRadiusInExplosion */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DamageRadius;


	/**************************************************************************/
	/* State */
	/**************************************************************************/
private:

	/** World Location of this actor last frame */
	FVector PreviousWorldLocation;

	/** World Location of this actor this frame  */
	FVector CurrentWorldLocation;

	/** Collision Radius in sphere trace for collisions, will be set to CollisionComp radius */
	float CollisionRadius;

	/** Has a hit be detected */
	bool bHitDetected;


/*
* Members *
*/
	
public:	
	// Sets default values for this actor's properties
	ATProjectile();

	virtual void Tick(float DeltaTime) override;

	/** Setters for damage parameters */
	FORCEINLINE void SetDamageAmountInExplosion(float Value) { DamageAmount = Value; }
	FORCEINLINE void SetDamageRadiusInExplosion(float Value) { DamageRadius = Value; }
	FORCEINLINE void SetDamageType(TSubclassOf<UDamageType> Type) { DamageType = Type; }

	/**
	 * Set Launch Velocity Vector for the projectile
	 * 
	 * @param LaunchVelocity	Velocity to launch projectile
	 * @param GravityScale		Gravity scale to apply to projectile
	 */
	void SetLaunchVelocityVector(FVector LaunchVelocity, float GravityScale);

	/** Set the max speed and initial speed to LaunchSpeed*/
	void SetLaunchSpeed(float LaunchSpeed);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/** Called at end HandleDestruction, uses timeline in derived BP base class to fade out TrailEffectComp */
	UFUNCTION(BlueprintImplementableEvent)
	void BP_FinishDestroyProjectile();

	/** Spawn VFX and SFX when collision is detected, will spawn at HitResult.Location */
	void EmitOnHitEffects(const FVector& Location) const;

private:

	/** Called when collision is detected, disables collisions, calls BP_FinishDestroyProjectile */
	void HandleDestruction();

	/** Sphere trace from world location last frame to world location this frame to detect collisions */
	bool DetectHit(FHitResult& HitResult);

	/** Apply radial damage when collision is detected */
	void ApplyDamage(FHitResult& HitResult);
};
