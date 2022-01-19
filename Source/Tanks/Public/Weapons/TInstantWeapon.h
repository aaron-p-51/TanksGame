// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TShootableWeapon.h"
#include "TInstantWeapon.generated.h"

class UNiagaraSystem;
class UAudioComponent;
class USoundBase;


/** Results from weapon trace, used to replicate FX over network */
USTRUCT(BlueprintType)
struct FHitScanTraceData
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> HitSurfaceType;

	UPROPERTY()
	FVector_NetQuantize HitLocation;

	UPROPERTY()
	FVector_NetQuantize HitDirection;

	void SetHitScanTraceValues(TEnumAsByte<EPhysicalSurface> Surface, FVector_NetQuantize Location, FVector_NetQuantize Direction)
	{
		HitSurfaceType = Surface;
		HitLocation = Location;
		HitDirection = Direction;
	}
};

/** Impact FX for differnet physics materials */
USTRUCT(BlueprintType)
struct FImpactEffects
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* ImpactSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* ParticleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UNiagaraSystem* NiagaraEffect;
	
};

/**
 * 
 */
UCLASS()
class TANKS_API ATInstantWeapon : public ATShootableWeapon
{
	GENERATED_BODY()

/*
* Members *
*/
protected:

	/**************************************************************************/
	/* Weapon Configuration */
	/**************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	bool bIsFullAutomatic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bUseBulletSpread;

	/** Bullet spread applied as a random cone from weapon fire point. Spread will be different on server and clients.
	 *  Spread should be small so game play experience is not degraded*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "bUseBulletSpread"))
	float BulletSpread;

	/** Distance for line trace to check if weapon hit another actor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	float LineTraceDistance;

	/**************************************************************************/
	/* Weapon FX */
	/**************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TMap<TEnumAsByte<EPhysicalSurface>, FImpactEffects> ImpactEffects;

	/** If unable to find EPhysicalSurface in ImpactEffects map then use this SFX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | Defaults")
	USoundBase* DefaultImpactSoundCue;

	/** If unable to find EPhysicalSurface in ImpactEffects map then use this VFX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | Defaults")
	UParticleSystem* DefaultImpactParticles;


	/**************************************************************************/
	/* State */
	/**************************************************************************/

	/** Replicated trace data used to spawn FX on non owning clients */
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTraceData HitScanTrace;

/* Methods */
public:
	ATInstantWeapon();

	


protected:


	/** [Server + Client] Player input fire weapon */
	virtual void FireWeapon() override;

	UFUNCTION(Server, Reliable)
	void ServerFire();

	/**
	 * [Server + Client] Process HitResult from @WeaponFireLineTrace, will spawn impact FX. Apply damage when run on server
	 * 
	 * @param Hit			HitRestul from WeaponFireLineTrace
	 * @param Origin		Location where line trace started
	 * @param Direction		Direction from Origin line trace was performed
	 */
	void ProcessWeaponLineTraceHit(const FHitResult& Hit, const FVector& Origin, const FVector& Direction);

	/**
	 * [Server + Client] Spawn impact SFX and VFX
	 * 
	 * @param Surface		Physics material of impact surface
	 * @param ImpactPoint	Location where impact occured
	 * @param Direction		Direction line trace was performed which resulted in blocking hit
	 */
	void PlayImpactEffects(TEnumAsByte<EPhysicalSurface> Surface, const FVector& ImpactPoint, const FVector& Direction) const;

	/** Play Impact FX from simulated proxies */
	UFUNCTION()
	void OnRep_HitScanTrace();

	/** [Server + Client] Set line trace end points for weapoon line trace*/
	void SetLineTraceEndPoints(FVector& LineTraceStart, FVector& LineTraceEnd) const;

	/** [Server + Client] Perform line trace store blocking hit in HitResult */
	bool WeaponFireLineTrace(FHitResult& HitResult, const FVector& LineTraceStart, const FVector& LineTraceEnd) const;

	/** [Server + Client] Apply bullet spread, spread will be different on clients and server, see @BulletSpread 
	 */
	FVector ApplyBulletSpread(const FVector& ShotDirection) const;

};
