// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "TShootableWeaponOwner.h"
#include "WheeledVehicle.h"
#include "TPlayerTank.generated.h"

/** Forward declarations */
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class ATShootableWeapon;
class USceneComponent;
class UTHealthComponent;
class UParticleSystem;
class UAudioComponent;
class USoundBase;
class ATInstantWeapon;
class ATProjectileWeapon;
class UTBoostComponent;
class UNiagaraComponent;
class UTItemData;

/** Declare delegate for when player changes weapon */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootableWeaponChange, ATShootableWeapon*, NewWeapon);



UENUM(BlueprintType)
enum class EFirePoint : uint8
{
	EFP_PlayerCamera	UMETA(DisplayName="PlayerCamera"),
	EFP_TurretMesh		UMETA(DisplayName="TurretMesh"),

	EFP_DEFAULT_MAX		UMETA(DisplayName="Max")
};


/**
 * 
 */
UCLASS()
class TANKS_API ATPlayerTank : public AWheeledVehicle, public ITShootableWeaponOwner
{
	GENERATED_BODY()

/*
* Members *
*/

public:

	/** [Server + Client] Event called when player changes weapon */
	UPROPERTY(BlueprintAssignable)
	FOnShootableWeaponChange OnShootableWeaponChange;


private:

	/**************************************************************************/
	/* Components */
	/**************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCameraComponent* CameraComp;

	/** Stabilize Camera's rotation (pitch and roll)  */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* CameraGimbleComp;

	/** SkeletalMesh for tank Turret. Turret yaw will follow player yaw input */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* TurretMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTHealthComponent* HealthComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	UTBoostComponent* BoostComp;

	/** Continuous SFX for tank engine, sound will increase in pitch as speed increases  */
	UPROPERTY(EditDefaultsOnly, Category = "Components | FX | SFX")
	UAudioComponent* EngineAudioComp;

	/** Continuous VFX for tank engine, spawn rate will increase as speed increases  */
	UPROPERTY(EditDefaultsOnly, Category = "Components | FX | VFX")
	UNiagaraComponent* EngineExhauseNiagaraComp;


	/**************************************************************************/
	/* Configuration */
	/**************************************************************************/

	/** TODO: Implement Flip */
	/*
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | FlipOver")
	float MinRollToAllowFlip;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration | FlipOver")
	float MinPitchToAllowFlip;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration | FlipOver")
	FVector FlipImpulse;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration | FlipOver")
	FVector FlipCenterOfMass;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration | FlipOver")
	float FlipTime;
	*/

	/** Prevent player from exceeding this pitch angle when changing look direction and rotation */
	UPROPERTY(EditAnywhere, Category = "Configuration | Camera")
	float MaxCameraPitchAngle;

	/** Spring arm lengths used by this PlayerTank. Changing view will switch between these lengths */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Camera")
	TArray<uint32> SpringArmLengths;

	/** Socket name on TurretMesh where weapons should fire from, and where FX are spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration | Weapons")
	FName WeaponAttachSocketName;


	/**************************************************************************/
	/* Weapon Inventory */
	/**************************************************************************/

	/** Class of all weapons this tank should spawn.  */
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ATShootableWeapon>> WeaponInventoryClasses;

	/** Inventory of all ATShootableWeapon's will be an instnace of each class of WeaponInventoryClass */
	UPROPERTY(Replicated)
	TArray<ATShootableWeapon*> WeaponInventory;

	/** Current weapon player has equipt */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	ATShootableWeapon* CurrentWeapon;


	/**************************************************************************/
	/* Effects */
	/**************************************************************************/

	/** Param name to control EngineAudioComp pitch */
	UPROPERTY(EditDefaultsOnly, Category = "FX | SFX")
	FName EngineAudioRPMParamName;

	/** Param name to control EngineExhauseNiagaraComp spawn rate  */
	UPROPERTY(EditDefaultsOnly, Category = "FX | VFX")
	FName EngineExhaustSpawnRateParamName;

	/** SFX to play when this PlayerTank dies. */
	UPROPERTY(EditDefaultsOnly, Category = "FX | SFX")
	USoundBase* DeathSoundCue;

	/** VFX to play when this PlayerTank dies */
	UPROPERTY(EditDefaultsOnly, Category = "FX | VFX")
	UParticleSystem* DeathExplosion;

	 
	/** When this PlayerTank dies play camera shake on owning players camera */
	UPROPERTY(EditDefaultsOnly, Category = "FX | Camera")
	TSubclassOf<UCameraShake> DeathCameraShake;

	/**
	 * If damage taken in a single hit is larger than BigHitMinDamage BitHitCameraShake will play
	 * on owning players camera
	 */
	UPROPERTY(EditDefaultsOnly, Category = "FX | Camera")
	float BigHitMinDamage;

	/** Camera shake to play if this PlayerTank receives a big hit. See BigHitMinDamage  */
	UPROPERTY(EditDefaultsOnly, Category = "FX | Camera")
	TSubclassOf<UCameraShake> BigHitCameraShake;


	/**************************************************************************/
	/* States */
	/**************************************************************************/

	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerSetTurretYaw)
	float PlayerSetTurretYaw;

	UPROPERTY(VisibleAnywhere)
	bool bInputApplyBoost;

	/** Current SpringArmLenth used in SpringArmLengths. See SpringArmLengths */
	UPROPERTY(VisibleAnywhere, Category = "Configuration | Camera")
	uint32 CurrentSpringArmLengthIndex;


/*
* Methods *
*/
public:
	ATPlayerTank();

public:

	virtual void Tick(float DeltaTime) override;

	/**************************************************************************/
	/* Setup */
	/**************************************************************************/

private:

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

private:
	
	/** [Server] Bind to member components events */
	void BindToComponentEvents();

	/** [Client] Stabilize camera, camera will not change its pitch or roll based on its attachment rotation */
	void StabilizeCameraGimble();

	/** [Client] Set starting spring arm length for camera. Will be first entry in SpringArmLengths TArray */
	void SetInitialSpringArmLength();

	/**************************************************************************/
	/* Weapon */
	/**************************************************************************/

public:

	/**
	 * [Server + Client] Get the location and rotation, out params, of where the fire point in for this ATPlayerTank.
	 * Line trace weapons will use player camera, projectile weapons will use TurretMeshComp's BarrelSocket.
	 * 
	 * TODO: More robust system for line trace weapons. Line trace should also trance from TurretMeshComp's BarrelSocket
	 * to esnure the hit location. prevent blocking hits from obsticels between camera and this ATPlayerTank.
	 */
	virtual void GetShootableWeaponFirePoint(FVector& FireLocation, FRotator& FireRotation) override;

	/** [Server + Client] Get Weapon attachment */
	virtual USceneComponent* GetAttachmentSceneComponent() override { return TurretMeshComp; }
	virtual FName GetAttachmentSocketName() override { return WeaponAttachSocketName; }

	/** [Client] Use current weapon */
	virtual void StartFire_Implementation() override;
	virtual void StopFire_Implementation() override;
	virtual bool IsFiring_Implementation() override;
	virtual void Reload_Implementation() override;
	
	/** [Server + Client] Weapon Type either Instant, or projectile */
	UFUNCTION(BlueprintCallable)
	EShootableWeaponType GetEquipedWeaponType() const;

	/** [Server + Client] Get current weapon from WeaponInventory use by player */
	UFUNCTION(BlueprintCallable)
	ATShootableWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	
	/**************************************************************************/
	/* Health */
	/**************************************************************************/

	/** Get reference to HealthComponent */
	UFUNCTION()
	UTHealthComponent* GetHealthComponent() const { return HealthComp; }

	/** [Server] Give health to this player tank. Returns actual amount of health this player tank took */
	UFUNCTION(BlueprintCallable)
	float GiveHealth(float Amount);

	/** [Server + Client] Get the Max heath this player tank can have */
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	/** [Server + Client] Get the current health of this player tank */
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;


	/** [Server] Bound to HealthComponents OnHealthChange Event. Will trigger when HealthComp's health value changes */
	UFUNCTION()
	void OnHealthCompHealthChange(float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser);


	/**************************************************************************/
	/* Boost */
	/**************************************************************************/


public:

	UFUNCTION()
	UTBoostComponent* GetBoostComponent() const { return BoostComp; }

	UFUNCTION(BlueprintCallable)
	float GetBoost() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxBoost() const;


public:
	UFUNCTION()
	void OnBoostComponentBoostChange(float Boost, bool UsingBoost);


	/*************************************************************************/
	/* Weapon Inventory */
	/*************************************************************************/

private:

	/**
	 * [Server] Spawn an instance of every weapon in WeaponInventoryClasses. Each weapon will be added to
	 * WeaponInventory. WeaponInventory is replicated to owning client.
	 */
	void SpawnDefaultWeaponInventory();

	/**
	 * [Server] Add Weapon to WeaponInventory. Only adds to WeaponInventory if Weapon is unique to other
	 * Weapons currently in WeaponInventory. 
	 */
	void AddWeapon(ATShootableWeapon* Weapon);

	/** [Server + Client] Equip Weapon.  Weapon must be in WeaponInventory. */
	void EquipWeapon(ATShootableWeapon* Weapon);

	/** Server RPC to equip weapon. Calls EquipWeapon on server. */
	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(ATShootableWeapon* Weapon);

	/** [Client] Switch to next weapon in WeaponInventory. Will cause next weapon in WeaponInventory to be equipped. */
	void SwitchToNextWeapon();

	/** [Client] Switch to previous weapon in WeaponInventory. Will cause previous weapon in WeaponInventoryto be equipped. */
	void SwitchToPreviousWeapon();

	/** [Server + Client] Sets the NewWeapon as current Weapon */
	void SetCurrentWeapon(ATShootableWeapon* NewWeapon, ATShootableWeapon* PreviousWeapon);

	/** [Server + Client] Broadcasts OnShootableWeaponChage event*/
	UFUNCTION()
	void OnRep_CurrentWeapon(ATShootableWeapon* LastWeapon);

	/**
	 * Server RPC to set AutonomousProxy's camera position and rotation on server. Position is used for weapon line trace's to determine hits
	 * 
	 * TODO: Currently this RPC is called on tick. Make this call only happen when player is firing weapon.
	 */
	UFUNCTION(Server, Unreliable)
	void ServerSetCameraForWeaponFire(FVector Pos, FRotator Rot);


	/** [Server + Client] Returns first ATShootableWeapon in WeaponInventory that uses ItemType */
	ATShootableWeapon* FindFirstWeaponUsesItem(EItemType ItemType) const;


	/*************************************************************************/
	/* Player Input */
	/*************************************************************************/

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:

	/** [Client] Move tank forward using Throttle Input on VehicleMovementComponent */
	void MoveForward(float Value);

	/** [Client] Steer the tank using Steering Input on VehicleMovementComponent */
	void MoveRight(float Value);

	/** [Client] Rotate Player View. Calculate and SprintArmComp rotation based on Value. Rotate TurretMeshComp  */
	/**
	 * [Client] Rotate Player View. Calculate and SprintArmComp rotation based on Value. Rotate TurretMeshComp based
	 * on the SprintArmComp's rotation. Calls ServerSetTurretYaw RPC to set TurretMeshComp on simulated proxies
	 */
	void LookRight(float Value);

	/** [Client] Rotate Player View. Calculate and SprintArmComp rotation based on Value. */
	void LookUp(float Value);

	/** [Client] Change camera distance from tank. Will change to next value in SpringArmLengths*/
	void ChangeCameraView();


	/** [Client] Start using boost */
	UFUNCTION()
	void StartUseBoost();

	/** [Client] Stop using boost */
	UFUNCTION()
	void StopUseBoost();

	void FlipUpright();

	/** [Client] Calls StartFire_Implementation. See StartFire_Implementation */
	void StartUseWeapon();

	/** [Client] Calls StopFire_Implementation. See StopFire_Implementation */
	void StopUseWeapon();

	/** [Client] Calls Reload_Implementation. See Reload_Implementation */
	void WeaponReload();

	/*************************************************************************/
	/* State */
	/*************************************************************************/

	UFUNCTION()
	void OnRep_IsDead();

	void PlayDeathFX();
	
	bool CanFlipOver() const;

	/** [Server + Client] Get GetVehicleMovementComponent's engine RPM */
	float GetEngineRotationSpeed() const;

	/** [Client] Set TurretComp's relative yaw on Simulated Proxies */
	UFUNCTION()
	void OnRep_PlayerSetTurretYaw();

	UFUNCTION(Server, Reliable)
	void ServerSetTurretYaw(float Value);
	
	/*************************************************************************/
	/* FX */
	/*************************************************************************/

	/** [Client] Player camera shake on owners Camera */
	void PlayCameraShake(TSubclassOf<UCameraShake> CameraShakeClass);

	/** [Client] Set pitch of EngineAudioComp based on VehicleMovementComponent's engine RPM */
	void SetEngineAudioFromRotationSpeed();

	/** [Client] Set spawn rate of EngineExhauseNiagaraComp based on VehicleMovementComponent's engine RPM */
	void SetEngineExhaustFromRotationSpeed();

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void EventSpawnFloatingText(AController* InstigatedBy, float Damage);


	/*************************************************************************/
	/* Items */
	/*************************************************************************/
public:

	/** [Server] Pickup item and try to apply its Quantity. Returns true if able to use item */
	bool PickupItem(EItemType ItemType, int32 Quantity);


};
