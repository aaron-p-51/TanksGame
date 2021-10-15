// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "GameFramework/Actor.h"
#include "TShootableWeapon.generated.h"

class UNiagaraSystem;
class UParticleSystem;
class UStaticMeshComponent;
class USoundBase;
class UAudioComponent;
class UImage;
class ATPlayerTank;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoAmountChange, int32, AmmoInCarry, int32, AmmoInMagazine);


UENUM(BlueprintType)
enum class EFireMode : uint8
{
	EFM_FullAutomatic		UMETA(DisplayName="Automatic"),
	EFM_SemiAutomatic		UMETA(DisplayName="SemiAuto"),

	EFM_MAX					UMETA(DisplayName="DefaultMax")
};


UENUM(BlueprintType)
enum class EShootableWeaponState : uint8
{
	ESWS_Idle,
	ESWS_Firing,
	ESWS_Reloading
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootableWeaponStateChange, EShootableWeaponState, State);


USTRUCT(BlueprintType)
struct FWeaponSpecs
{
	GENERATED_BODY()


};


UCLASS(Abstract, Blueprintable)
class TANKS_API ATShootableWeapon : public AActor
{
	GENERATED_BODY()

/*
* Members *
*/
public:

	UPROPERTY(BlueprintAssignable)
	FOnAmmoAmountChange OnAmmoAmountChange;

	UPROPERTY(BlueprintAssignable)
	FOnShootableWeaponStateChange OnShootableWeaponStateChange;
	

protected:

	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	ATPlayerTank* MyPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USceneComponent* FirePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bOwnerSetsFirePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bOwnerSetsShotDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Type")
	EShootableWeaponType ShootableWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bWantsToFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bWantsToReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ReloadTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UNiagaraSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* MuzzleFlash_Particles;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | UI")
	UMaterial* WeaponIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | UI")
	UTexture2D* WeaponReticle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo | Magazine")
	bool bInfinateAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo | Magazine")
	int32 MaxAmmoInCarry;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon | Ammo | Magazine")
	int32 CurrentAmmoInCarry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon | Ammo | Magazine")
	int32 MaxAmmoInMagazine;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon | Ammo | Magazine")
	int32 CurrentAmmoInMagazine;







	/*************************************************************************************
	*	Sound Effects
	**************************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SFX")
	USoundBase* FireSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SFX")
	USoundBase* ReloadSFX;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EFireMode FireMode;

	/* Rounds per minute fired by weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float RateOfFire;

	/* Derived from RateOfFire */
	float TimeBetweenShots;

	/* Last Time the weapon was fired */
	float LastFireTime;

	/* Timer for time between shots for if FireMode == EFM_FullAutomatic */
	FTimerHandle TimerHandle_HandleFiring;

	FTimerHandle TimerHandle_HandleReload;

	/** Adjustment to handle frame rate affecting actual timer interval. */
	UPROPERTY(Transient)
	float TimerIntervalAdjustment;

	/** Whether to allow automatic weapons to catch up with shorter refire cycles */
	UPROPERTY(Config)
	bool bAllowAutomaticWeaponCatchup = true;

	EShootableWeaponState CurrentState;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstFireCounter)
	int32 BurstFireCounter;

	/*UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	APawn* MyPawn;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DamageAmount;


/*
* Methods *
*/
	
public:
	// Sets default values for this actor's properties
	ATShootableWeapon();

	void SetOwningPawn(ATPlayerTank* NewOwner);

	UFUNCTION()
	void OnEquip();

	UFUNCTION()
	void OnEnterInventory(ATPlayerTank* NewOwner);

	//FORCEINLINE APawn* GetOwningPawn() const { return WeaponOwnerPawn; }



	FORCEINLINE void SetFirePoint(USceneComponent* Point);

	FVector GetFirePointLocation() const;

	FRotator GetFirePointRotation() const;

	UFUNCTION(BlueprintPure)
	FORCEINLINE UMaterial* GetWeaponIcon() const { return WeaponIcon; }


	UFUNCTION(BlueprintPure)
	FORCEINLINE UTexture2D* GetWeaponReticle() const { return WeaponReticle; }





	/* (Local + Server) Start Firing the weapon */
	virtual void StartFire();

	/* (Local + Server) Stop Firing the weapon */
	virtual void StopFire();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EShootableWeaponState GetCurrentWeaponState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EShootableWeaponType GetWeaponType() const { return ShootableWeaponType; }

	
	virtual void Reload();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	int32 GetCurrentAmmoInMagazine() const { return CurrentAmmoInMagazine; }
	int32 GetCurrentAmmoInCarry() const { return CurrentAmmoInCarry; }
	int32 GetMaxAmmoInMagazine() const { return MaxAmmoInMagazine; }
	int32 GetMaxAmmoInCarry() const { return MaxAmmoInCarry; }



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SimulateWeaponFire();

	void StopSimulatingWeaponFire();

	virtual void FireWeapon() PURE_VIRTUAL(ATShootableWeapon::FireWeapon, );

	void HandleWeaponFiring();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandleWeaponFiring();

	void HandleReFireWeapon();

	/* (local + server) Weapon Firing Finished */
	virtual void BurstFireFinished();

	/* (local + server) Weapon Firing Started */
	virtual void BurstFireStarted();

	bool CanFire() const;

	UFUNCTION()
	virtual void OnRep_BurstFireCounter();

	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerSetOwningPawn(APawn* Pawn);

	void ReloadStarted();

	void HandleWeaponReload();




	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();



	void DetermineCurrentWeaponState();

	void SetCurrentWeaponState(EShootableWeaponState NewState);


	UFUNCTION()
	void OnLeaveInventory();

	UFUNCTION()
	void OnRep_MyPawn();

	UFUNCTION()
	void AttachMeshToPawn();

};
