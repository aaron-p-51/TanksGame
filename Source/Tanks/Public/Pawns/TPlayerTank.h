// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "TShootableWeaponOwner.h"
#include "WheeledVehicle.h"
#include "TPlayerTank.generated.h"


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

	UPROPERTY(BlueprintAssignable)
	FOnShootableWeaponChange OnShootableWeaponChange;

protected:



	/**************************************************************************/
	/* Components */
	/**************************************************************************/

	/** SkeletalMesh for tank Turret. Turret yaw will follow player yaw input */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* TurretMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTHealthComponent* HealthComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UTBoostComponent* BoostComp;

	/** Continuous SFX for tank engine, sound will increase in pitch as speed increases  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components | FX | SFX")
	UAudioComponent* EngineAudioComp;

	/** Continuous VFX for tank engine, spawn rate will increase as speed increases  */
	UPROPERTY(EditDefaultsOnly, Category = "Components | FX | VFX")
	UNiagaraComponent* EngineExhauseNiagaraComp;

	UWheeledVehicleMovementComponent* VehicleMovementComp;


	/**************************************************************************/
	/* Weapon Inventory */
	/**************************************************************************/

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ATShootableWeapon>> WeaponInventoryClasses;

	UPROPERTY(Transient, Replicated)
	TArray<ATShootableWeapon*> WeaponInventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	ATShootableWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	FName WeaponAttachSocketName;


	/**************************************************************************/
	/* Effects */
	/**************************************************************************/

	UPROPERTY(EditDefaultsOnly, Category = "FX | SFX")
	FName EngineAudioRPMParamName;

	UPROPERTY(EditDefaultsOnly, Category = "FX | VFX")
	FName EngineExhaustSpawnRateParamName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | SFX")
	USoundBase* DeathSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | VFX")
	UParticleSystem* DeathExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | Camera")
	TSubclassOf<UCameraShake> DeathCameraShake;

	/** If damage taken in a single hit is larger than BigHitMinDamage BitHitCameraShake will be used */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | Camera")
	float BigHitMinDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX | Camera")
	TSubclassOf<UCameraShake> BigHitCameraShake;

	/**************************************************************************/
	/* States */
	/**************************************************************************/

	UPROPERTY(ReplicatedUsing=OnRep_SetDesiredTurretYaw)
	float DesiredTurretYaw;

	UPROPERTY(ReplicatedUsing=OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool bIsFlipping;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bInputApplyBoost;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bAILookAtTarget;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	AActor* AILookAtTarget;

	uint32 CurrentSpringArmLengthIndex;

	UPROPERTY(EditDefaultsOnly)
	TArray<uint32> SpringArmLengths;

	/**************************************************************************/
	/* Configuration */
	/**************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | FlipOver")
	float MinRollToAllowFlip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | FlipOver")
	float MinPitchToAllowFlip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | FlipOver")
	FVector FlipImpulse;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | FlipOver")
	FVector FlipCenterOfMass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration | FlipOver")
	float FlipTime;



	UPROPERTY()
	FVector DefaultCenterOfMass;






/*
* Methods *
*/
public:
	ATPlayerTank();




	UFUNCTION(BlueprintCallable)
	void GetWeaponFirePoint(FVector& FirePointLocation, FRotator& FirePointRotation, EFirePoint FirePoint = EFirePoint::EFP_PlayerCamera);

	
	virtual void GetShootableWeaponFirePoint(FVector& FireLocation, FRotator& FireRotation) override;


	UFUNCTION(BlueprintCallable)
	void GiveHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION()
	UTHealthComponent* GetHealthComponent() const { return HealthComp; }

	UFUNCTION(BlueprintCallable)
	void EnableAILookAtTarget(bool Enable, AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool IsAILookAtTargetEnabled() const {return bAILookAtTarget;}


	/**
	 * TODO: Verify Interface is really needed for these three functions
	 */
	virtual void StartFire_Implementation() override;
	virtual void StopFire_Implementation() override;
	virtual bool IsFiring_Implementation() override;

	void WeaponReload();


	UFUNCTION(BlueprintCallable)
	EShootableWeaponType GetEquipedWeaponType() const;


	UFUNCTION(BlueprintCallable)
	ATShootableWeapon* GetCurrentWeapon() const;

	FORCEINLINE FName GetWeaponSocketName() const { return WeaponAttachSocketName; }

	FORCEINLINE USkeletalMeshComponent* GetTurretMeshComp() const { return TurretMeshComp; }


protected:

	virtual void BeginPlay() override;



	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/*************************************************************************/
	/* Weapon Inventory */
	/*************************************************************************/

	void SpawnDefaultInventory();

	void AddWeapon(ATShootableWeapon* Weapon);

	void EquipWeapon(ATShootableWeapon* Weapon);

	void SwitchToNextWeapon();

	void SwitchToPreviousWeapon();

	void FlipUpright();

	UFUNCTION(Server, Reliable)
	void ServerFlipUpright();

	FTimerHandle TimerHandle_FlipOver;

	void FlipTimerComplete();


	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(ATShootableWeapon* Weapon);

	void SetCurrentWeapon(ATShootableWeapon* NewWeapon, ATShootableWeapon* PreviousWeapon);

	UFUNCTION()
	void OnRep_CurrentWeapon(ATShootableWeapon* LastWeapon);

	/*************************************************************************/
	/* Player Input */
	/*************************************************************************/

	/** [Client] Move tank forward */
	void MoveForward(float Value);

	/** [Client] Steering input for tank */
	void MoveRight(float Value);

	/** [Client] Rotate tank turret */
	void LookRight(float Value);

	/** [Server] Set local controlled tank turret rotation on server */
	UFUNCTION(Server, Unreliable)
	void ServerSetDesiredTurretYaw(float YawValue);

	/** [Client] Aim up/down */
	void LookUp(float Value);

	/** [Client] Change camera distance from tank*/
	void ChangeCameraView();

	/** [Client] Start using boost */
	UFUNCTION()
	void StartUseBoost();

	/** [Client] Stop using boost */
	UFUNCTION()
	void StopUseBoost();



	UFUNCTION()
	void OnRep_IsDead();

	void PlayDeathFX();
	
	void InformControllerHealthChange(class AController* InstigatedBy, float NewHealth, float HealthDelta, bool IsDead);

	void PlayCameraShake(TSubclassOf<UCameraShake> CameraShakeClass);
	
	void SetEngineAudioFromRotationSpeed();

	void SetEngineExhaustFromRotationSpeed();
	


	UFUNCTION()
	void OnRep_SetDesiredTurretYaw();

	/** [Server] Delegate triggered from Health Component broadcast */
	UFUNCTION()
	void OnHealthCompHealthChange(UTHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser);


	UFUNCTION()
	void OnBoostComponentBoostChange(float Boost, bool UsingBoost);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_DebugBoostApplied(float Boost, bool UsingBoost);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetThrottleTest(float Value);





	


	UFUNCTION(BlueprintImplementableEvent)
	void EventSpawnFloatingText(AController* InstigatedBy, float Damage);

	/** Used to Get Fire Position, TODO: if fails from Reliable buffer overflow change approach */
	UFUNCTION(Server, Reliable)
	void ServerSetSpringArmRotation(FVector Value);

	
	



private:

	virtual void Tick(float DeltaTime) override;

	void AIRotateTurretToFaceTarget();

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

	bool CanFlipOver() const;


};
