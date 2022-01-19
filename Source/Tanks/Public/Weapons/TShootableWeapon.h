// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "GameFramework/Actor.h"
#include "TShootableWeapon.generated.h"


/** Forward delcarations */
class UNiagaraSystem;
class UParticleSystem;
class UStaticMeshComponent;
class USoundBase;
class UAudioComponent;
class UImage;
class ATPlayerTank;


/**
 * Weapon usage mode. Controls how player uses weapon. EFM_FullAutomatic is intended to continue firing
 * once player uses Fire Action Mapping until they stop. EFM_SemiAutomatic will only fire once per
 * player using Fire Action Mapping.
 */
UENUM(BlueprintType)
enum class EFireMode : uint8
{
	EFM_FullAutomatic		UMETA(DisplayName="Automatic"),
	EFM_SemiAutomatic		UMETA(DisplayName="SemiAuto"),

	EFM_MAX					UMETA(DisplayName="DefaultMax")
};


/** Current stat of this weapon, player input will change state */
UENUM(BlueprintType)
enum class EShootableWeaponState : uint8
{
	ESWS_Idle,
	ESWS_Firing,
	ESWS_Reloading
};


/** delegate for weapon state change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootableWeaponStateChange, EShootableWeaponState, State);
/** delegate for when ammo amounts change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoAmountChange, int32, AmmoInCarry, int32, AmmoInMagazine);


/**
 * Base class for shootable weapons.
 */
UCLASS(Abstract, Blueprintable)
class TANKS_API ATShootableWeapon : public AActor
{
	GENERATED_BODY()

/*
* Members *
*/
public:

	/** Event for ammo amounts changing */
	UPROPERTY(BlueprintAssignable)
	FOnAmmoAmountChange OnAmmoAmountChange;

	/** Event for weapon state changing */
	UPROPERTY(BlueprintAssignable)
	FOnShootableWeaponStateChange OnShootableWeaponStateChange;
	

	/**************************************************************************/
	/* Components */
	/**************************************************************************/

private:

	/** Mesh component for this weapon. Does not need to be set if attached to owner component directily */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	/** SceneComponent where this weapon is attached to on owner. Should be scene component on MyPawn */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* OwnersAttachToSceneComp;

protected:

	/** Point where weapon will shoot from */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USceneComponent* FirePoint;


	/**************************************************************************/
	/* Configuration */
	/**************************************************************************/

private:

	/** Weapon does not use ammo */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Ammo")
	bool bInfinateAmmo;

	/** Max amount of ammo this weapon can have in its carry. When reloading ammo will go from Carry to Magazine  */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Ammo")
	int32 MaxAmmoInCarry;

	/** Current amount of ammo in its carry */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmoInCarry, EditDefaultsOnly, Category = "Configuration|Ammo")
	int32 CurrentAmmoInCarry;

	/** Max amount of ammo this weapon can have loaded at any one time */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Ammo")
	int32 MaxAmmoInMagazine;

	/** Current amount of ammo loaded */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmoInMagazine, EditDefaultsOnly, Category = "Configuration|Ammo")
	int32 CurrentAmmoInMagazine;

	/** Item (ammo type) this weapon uses when firing */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Item")
	EItemType ConsumableItem;

	/** Icon to identify weapon on player HUD  */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|UI")
	UMaterial* WeaponIcon;

	/** Aim point of player HUD */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|UI")
	UTexture2D* WeaponReticle;

protected:

	/** Type of damage caused by this weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Damage")
	TSubclassOf<UDamageType> DamageType;

	/** Amount of damage to apply to hit actor */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration|Damage")
	float DamageAmount;

	UPROPERTY(VisibleAnywhere, Category = "Configuration")
	EShootableWeaponType ShootableWeaponType;

	/** If true get the firepoint location from the owner. Owner must use inherit ITShootableWeaponOwner  */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bOwnerSetsFirePoint;

	/** If true get the firepoint rotation from the owner. Owner must use inherit ITShootableWeaponOwner */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bOwnerSetsShotDirection;

private:

	/** Amount of time to reload weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float ReloadTime;

	/* Rounds per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float RateOfFire;

	/* Derived from RateOfFire */
	float TimeBetweenShots;

	/* Last Time the weapon was fired */
	float LastFireTime;

	/** Weapon fire mode */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	EFireMode FireMode;

	/** Whether to allow automatic weapons to catch up with shorter refire cycles */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bAllowAutomaticWeaponCatchup = true;

	/** If attaching to owning pawn scene component socket name to attach to on owning pawns scene component */
	UPROPERTY(VisibleAnywhere)
	FName OwnersAttachToSocketName;


	/**************************************************************************/
	/* State */
	/**************************************************************************/

	/** Weapon wants to fire and will fire next TimerHandle_HandleFiring cycle */
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bWantsToFire;

	/** State of weapon, only valid on server */
	UPROPERTY(VisibleAnywhere, Category = "State")
	EShootableWeaponState CurrentState;

	/** Weapon wants to reload when set to true weapon will start to reload, is set back to false when reload is complete */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	bool bWantsToReload;

	/* Timer handle for time between shots for if FireMode == EFM_FullAutomatic */
	FTimerHandle TimerHandle_HandleFiring;

	/** Timer handle for reload time */
	FTimerHandle TimerHandle_HandleReload;

	/** Adjustment to handle frame rate affecting actual timer interval. */
	UPROPERTY(Transient)
	float TimerIntervalAdjustment;

	/** Replicated counter to signal simulated proxies to fire their weapons */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstFireCounter)
	int32 BurstFireCounter;

protected:

	/** This weapons owning Pawn  */
	UPROPERTY(Transient, VisibleAnywhere, ReplicatedUsing = OnRep_MyPawn)
	APawn* MyPawn;


	/**************************************************************************/
	/* FX */
	/**************************************************************************/

private:

	/** Sound to play when weapon fires. */
	UPROPERTY(EditDefaultsOnly, Category = "FX|SFX")
	USoundBase* FireSFX;

	/** Sound to play when weapon reloads */
	UPROPERTY(EditDefaultsOnly, Category = "FX|SFX")
	USoundBase* ReloadSFX;

	/** VFX for weapon muzzle played when weapon fires */
	UPROPERTY(EditDefaultsOnly, Category = "FX|VFX")
	UNiagaraSystem* MuzzleFlash;

	/** VFX for weapon muzzle played when weapon fires */
	UPROPERTY(EditDefaultsOnly, Category = "FX|VFX")
	UParticleSystem* MuzzleFlash_Particles;


/*
* Methods *
*/
	
	/**************************************************************************/
	/* Setup */
	/**************************************************************************/

public:

	// Sets default values for this actor's properties
	ATShootableWeapon();

	/** [Server] Set MyPawn to NewOwner if different. Will set NewOwner as Owner and Instigator */
	void SetOwningPawn(APawn* NewOwner);

	/** [Server] Setup the attachment of this Weapon to its owning Pawn */
	void OnEquip();

	/** [Server + Client] Attach this weapon to MyPawn.  */
	void AttachMeshToPawn();

	/** [Server] Call from owning pawn when spawning this weapon. */
	void OnEnterInventory(APawn* NewOwner);

	/** TODO: Weapons currently no not evey leave inventory. May not need to implement  */
	void OnLeaveInventory();

	/** Owning pawn replicated from server  */
	UFUNCTION()
	void OnRep_MyPawn();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/**************************************************************************/
	/* Weapon Usage (Player Input) */
	/**************************************************************************/

public:

	/**
	 * [Server + Client] Start Firing the weapon. Will cause CurrentState to go into Firing state. CurrentState will remain in Firing State
	 * until StopFire() is called. Weapon FireMode will have no effect of returning to Idle State. ie if FireMode = EFM_SemiAutomatic
	 * FireState will remain Firing until StopFireIsCalled
	 */
	virtual void StartFire();

	/* [Server + Client] Stop Firing the weapon. Will cause CurrentState to go into Idle state. */
	virtual void StopFire();

	/** [Server + Client] Start reloading weapon. Will cause CurrentState to go into Reloading state. */
	virtual void Reload();

private:

	/** Server RPC for StartFire. Calls StartFire on server */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();

	/** Server RPC for StopFire. Calls StopFire on server */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	/** Server RPC for Reload. Calls Reload on server */
	UFUNCTION(Server, Reliable)
	void ServerReload();

	/**************************************************************************/
	/* Weapon State */
	/**************************************************************************/

public:

	/** [Server + Client] Get current weapon state */
	UFUNCTION(BlueprintCallable)
		FORCEINLINE EShootableWeaponState GetCurrentWeaponState() const { return CurrentState; }

	/** [Server + Client] Get Weapon Type  */
	UFUNCTION(BlueprintCallable)
		FORCEINLINE EShootableWeaponType GetWeaponType() const { return ShootableWeaponType; }

	/** Get Weapon Icon, used for weapon identification on player UI  */
	UFUNCTION(BlueprintPure)
		FORCEINLINE UMaterial* GetWeaponIcon() const { return WeaponIcon; }

	/** Get WeaponReticle, used for weapon aim point on player UI  */
	UFUNCTION(BlueprintPure)
		FORCEINLINE UTexture2D* GetWeaponReticle() const { return WeaponReticle; }

	/** [Server + Client] Get current amounts */
	FORCEINLINE int32 GetCurrentAmmoInMagazine() const { return CurrentAmmoInMagazine; }
	FORCEINLINE int32 GetCurrentAmmoInCarry() const { return CurrentAmmoInCarry; }
	FORCEINLINE int32 GetMaxAmmoInMagazine() const { return MaxAmmoInMagazine; }
	FORCEINLINE int32 GetMaxAmmoInCarry() const { return MaxAmmoInCarry; }
	FORCEINLINE EItemType GetConsumableItem() const { return ConsumableItem; }

private:

	/**
	 * [Server + Client] Based in current ammo limits and bWantsToFire, and bWantsToReload determine CurrentState.
	 * Does not set weapon state. See SetCurrentWeaponState.
	 */
	void DetermineCurrentWeaponState();

	/** [Server + Client] Set new weapon state. Calls methods required on state changes */
	void SetCurrentWeaponState(EShootableWeaponState NewState);

	/**************************************************************************/
	/* Weapon Fire */
	/**************************************************************************/

	/* [Server + Client] Weapon Firing Started */
	virtual void BurstFireStarted();

	/**
	 * [Server + Client] Process each weapon fire iteration. Decrease ammo amount, increment BurstFireCounter, Simulate weapon fire
	 * and setup next fire time. (TimerHandle_HandleFiring)
	 */
	void HandleWeaponFiring();

	/** Server RPC for HandleWeaponFiring. Increments BurstFireCounter   */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandleWeaponFiring();

	/** [Server + Client] Must be implemented in derived classes. Fire weapon and process damage as needed  */
	virtual void FireWeapon() PURE_VIRTUAL(ATShootableWeapon::FireWeapon, );

	/** [Server + Client] Setup to HandleWeaponFiring. Adjust TimerIntervalAdjustment based on time since last fire and TimeBetweenShots   */
	void HandleReFireWeapon();

	/** [Clients] Simulate the weapon firing. Plays weapon fire VFX and SFX. Plays on both Autonomous and Simulates proxies  */
	void SimulateWeaponFire();

	/** TODO: Not implemented. May not be needed */
	void StopSimulatingWeaponFire();

	/* (local + server) Weapon Firing Finished */
	virtual void BurstFireFinished();

	/** Gets if weapon wants to fire. Will remain true from when StartFire() is called until StopFire() is called  */
	FORCEINLINE bool CanFire() const { return bWantsToFire; }

	/** BurstFireCounter replicated. SimulateWeaponFire for simulated proxies */
	UFUNCTION()
	virtual void OnRep_BurstFireCounter();


	/**************************************************************************/
	/* Weapon Reload */
	/**************************************************************************/

	/** [Server + Client] Start weapon reloading. Weapon will not finish reloading until HandleWeaponReload is called  */
	void ReloadStarted();

	/** [Server + Client] Reload weapon. Move ammo from CurrentAmmoInCarry to CurrentAmmoInMagazine */
	void HandleWeaponReload();


	/**************************************************************************/
	/* Ammo */
	/**************************************************************************/

public:

	/**
	 * [Server] Add Ammo. Ammo will be added to CurrentAmmoInCarry up to MaxAmmoInCarry. Returns the actual amount of
	 * ammo added to CurrentAmmoInCarry. Will return 0 if called from client.
	 */
	int32 AddAmmo(int32 Amount);

private:

	/** Ammo in Carry replicated. Broadcast OnAmmoAmountChange event */
	UFUNCTION()
	void OnRep_CurrentAmmoInCarry();

	/** Ammo in Magazine replicated. Broadcast OnAmmoAmountChange event */
	UFUNCTION()
	void OnRep_CurrentAmmoInMagazine();

	/**************************************************************************/
	/* FX */
	/**************************************************************************/

	/** Play MuzzleFlash and/or MuzzleFlash_Particles VFX. Will be spawned attached to FirePoint */
	void PlayWeaponFireVFX() const;

	/** Play FireSFX at current location of the ShootableWeapon */
	void PlayWeaponFireSFX() const;

	/** Play ReloadSFX at current location of the ShootableWeapon. Only plays on locally controlled pawn */
	void PlayWeaponReloadSFX() const;



};
