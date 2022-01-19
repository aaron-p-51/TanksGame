// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TShootableWeapon.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Game Includes
#include "Pawns/TPlayerTank.h"


// Sets default values
ATShootableWeapon::ATShootableWeapon()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (MeshComp)
	{
		SetRootComponent(MeshComp);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	if (FirePoint)
	{
		FirePoint->SetupAttachment(GetRootComponent());
	}

	RateOfFire = 600.0f;
	bWantsToFire = false;
	bWantsToReload = false;
	ReloadTime = 2.f;
	CurrentState = EShootableWeaponState::ESWS_Idle;
	BurstFireCounter = 0;
	
	SetReplicates(true);
	MinNetUpdateFrequency = 33.0f;
	NetUpdateFrequency = 66.0f;

	ShootableWeaponType = EShootableWeaponType::ESWT_DefaultMax;
}

/**************************************************************************/
/* Setup */
/**************************************************************************/
// Called when the game starts or when spawned
void ATShootableWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.f / RateOfFire;
}


void ATShootableWeapon::SetOwningPawn(APawn* NewOwner)
{
	if (NewOwner != MyPawn && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}
}


void ATShootableWeapon::OnEquip()
{
	AttachMeshToPawn();
}

void ATShootableWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		ITShootableWeaponOwner* ShootableWeaponOwner = Cast<ITShootableWeaponOwner>(MyPawn);
		if (ShootableWeaponOwner)
		{
			OwnersAttachToSceneComp = ShootableWeaponOwner->GetAttachmentSceneComponent();
			OwnersAttachToSocketName = ShootableWeaponOwner->GetAttachmentSocketName();
			if (OwnersAttachToSceneComp)
			{
				GetRootComponent()->AttachToComponent(OwnersAttachToSceneComp, FAttachmentTransformRules::KeepRelativeTransform, OwnersAttachToSocketName);
			}
		}

		// TODO: If there is a MeshComp for weapon then enable its visibility
	}
}


void ATShootableWeapon::OnEnterInventory(APawn* NewOwner)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		SetOwningPawn(NewOwner);
	}
}


void ATShootableWeapon::OnLeaveInventory()
{

}


void ATShootableWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}


/**************************************************************************/
/* Weapon Usage (Player Input) */
/**************************************************************************/
void ATShootableWeapon::StartFire()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineCurrentWeaponState();
	}
}


void ATShootableWeapon::ServerStartFire_Implementation()
{
	StartFire();
}


bool ATShootableWeapon::ServerStartFire_Validate()
{
	return true;
}


void ATShootableWeapon::StopFire()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineCurrentWeaponState();
	}
}


void ATShootableWeapon::ServerStopFire_Implementation()
{
	StopFire();
}


bool ATShootableWeapon::ServerStopFire_Validate()
{
	return true;
}


void ATShootableWeapon::Reload()
{
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerReload();
	}

	bWantsToReload = true;
	DetermineCurrentWeaponState();
}


void ATShootableWeapon::ServerReload_Implementation()
{
	Reload();
}


/**************************************************************************/
/* Weapon State */
/**************************************************************************/
void ATShootableWeapon::DetermineCurrentWeaponState()
{
	EShootableWeaponState NewState = EShootableWeaponState::ESWS_Idle;

	if (!bWantsToFire && bWantsToReload && CurrentAmmoInCarry > 0 && CurrentAmmoInMagazine < MaxAmmoInMagazine)
	{
		NewState = EShootableWeaponState::ESWS_Reloading;
	}
	else if (bWantsToFire && (CurrentAmmoInMagazine > 0 || bInfinateAmmo))
	{
		NewState = EShootableWeaponState::ESWS_Firing;
	}
	else
	{
		NewState = EShootableWeaponState::ESWS_Idle;
	}

	SetCurrentWeaponState(NewState);
}


void ATShootableWeapon::SetCurrentWeaponState(EShootableWeaponState NewState)
{
	const EShootableWeaponState OldState = CurrentState;

	if (OldState == EShootableWeaponState::ESWS_Idle && NewState == EShootableWeaponState::ESWS_Reloading)
	{
		ReloadStarted();
		CurrentState = EShootableWeaponState::ESWS_Reloading;
	}
	else if (OldState == EShootableWeaponState::ESWS_Firing && NewState != EShootableWeaponState::ESWS_Firing)
	{
		CurrentState = EShootableWeaponState::ESWS_Idle;
		BurstFireFinished();
	}
	else if (OldState != EShootableWeaponState::ESWS_Firing && NewState == EShootableWeaponState::ESWS_Firing)
	{
		CurrentState = EShootableWeaponState::ESWS_Firing;
		BurstFireStarted();
	}

	if (CurrentState != OldState)
	{
		OnShootableWeaponStateChange.Broadcast(CurrentState);
	}
}


/**************************************************************************/
/* Weapon Fire */
/**************************************************************************/
void ATShootableWeapon::BurstFireStarted()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if ((LastFireTime > 0) && (TimeBetweenShots > 0.0f) && (LastFireTime + TimeBetweenShots > GameTime))
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ATShootableWeapon::HandleWeaponFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleWeaponFiring();
	}
}


void ATShootableWeapon::HandleWeaponFiring()
{
	if (MyPawn)
	{
		if (CanFire())
		{
			if (GetNetMode() != NM_DedicatedServer)
			{
				// play fire FX locally
				SimulateWeaponFire();
			}

			if (MyPawn && MyPawn->IsLocallyControlled())
			{
				FireWeapon();
				CurrentAmmoInMagazine--;


				OnAmmoAmountChange.Broadcast(CurrentAmmoInCarry, CurrentAmmoInMagazine);

				// Incrementing BurstFireCounter here will only matter if called from Listen Server
				// Other clients reach to play the SimulatedWeaponFire for their version of this actor
				BurstFireCounter++;
			}
		}
		else if (MyPawn && MyPawn->IsLocallyControlled())
		{
			if (BurstFireCounter > 0)
			{
				BurstFireFinished();
			}
		}
		else
		{
			BurstFireFinished();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			// local client tell server it wants to fire
			if (GetLocalRole() < ENetRole::ROLE_Authority)
			{
				ServerHandleWeaponFiring();
			}

			if ((FireMode == EFireMode::EFM_FullAutomatic) && (CurrentState == EShootableWeaponState::ESWS_Firing) && (TimeBetweenShots > 0.0f) && (CurrentAmmoInMagazine > 0 || bInfinateAmmo))
			{
				// Calculate Refire TIme
				GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ATShootableWeapon::HandleReFireWeapon, FMath::Max<float>(TimeBetweenShots + TimerIntervalAdjustment, SMALL_NUMBER), false);
				TimerIntervalAdjustment = 0.f;
			}
			else
			{
				BurstFireFinished();
			}
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}


void ATShootableWeapon::ServerHandleWeaponFiring_Implementation()
{
	HandleWeaponFiring();

	// update fire FX on remote clients
	BurstFireCounter++;
}


bool ATShootableWeapon::ServerHandleWeaponFiring_Validate()
{
	return true;
}


void ATShootableWeapon::HandleReFireWeapon()
{
	UWorld* World = GetWorld();
	if (World)
	{
		float SlackTimeThisFrame = FMath::Max(0.0f, (World->TimeSeconds - LastFireTime) - TimeBetweenShots);

		if (bAllowAutomaticWeaponCatchup)
		{
			TimerIntervalAdjustment -= SlackTimeThisFrame;
		}

		HandleWeaponFiring();
	}
}


void ATShootableWeapon::SimulateWeaponFire()
{
	PlayWeaponFireVFX();
	PlayWeaponFireSFX();
}


void ATShootableWeapon::StopSimulatingWeaponFire()
{

}


void ATShootableWeapon::BurstFireFinished()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	BurstFireCounter = 0;
	TimerIntervalAdjustment = 0.0f;
}


void ATShootableWeapon::OnRep_BurstFireCounter()
{
	if (BurstFireCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}


/**************************************************************************/
/* Weapon Reload */
/**************************************************************************/
void ATShootableWeapon::ReloadStarted()
{
	// Reload will complete when reload timer elapses
	GetWorldTimerManager().SetTimer(TimerHandle_HandleReload, this, &ATShootableWeapon::HandleWeaponReload, ReloadTime, false);

	PlayWeaponReloadSFX();
}


void ATShootableWeapon::HandleWeaponReload()
{
	int32 ReloadAmount = MaxAmmoInMagazine - CurrentAmmoInMagazine;
	if (CurrentAmmoInCarry >= ReloadAmount)
	{
		CurrentAmmoInMagazine = MaxAmmoInMagazine;
		CurrentAmmoInCarry -= ReloadAmount;
	}
	else
	{
		CurrentAmmoInMagazine += CurrentAmmoInCarry;
		CurrentAmmoInCarry = 0;
	}

	bWantsToReload = false;
	OnAmmoAmountChange.Broadcast(CurrentAmmoInCarry, CurrentAmmoInMagazine);
	UE_LOG(LogTemp, Warning, TEXT("ReloadFinished"));
}


/**************************************************************************/
/* Ammo */
/**************************************************************************/
int32 ATShootableWeapon::AddAmmo(int32 Ammount)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && CurrentAmmoInCarry < MaxAmmoInCarry)
	{
		float AddedAmmoAmout = (CurrentAmmoInCarry + Ammount) > MaxAmmoInCarry ? MaxAmmoInCarry - CurrentAmmoInCarry : Ammount;
		CurrentAmmoInCarry = FMath::Min(Ammount + CurrentAmmoInCarry, MaxAmmoInCarry);

		OnAmmoAmountChange.Broadcast(CurrentAmmoInCarry, CurrentAmmoInMagazine);

		return AddedAmmoAmout;
	}

	return 0;
}


void ATShootableWeapon::OnRep_CurrentAmmoInCarry()
{
	OnAmmoAmountChange.Broadcast(CurrentAmmoInCarry, CurrentAmmoInMagazine);
}


void ATShootableWeapon::OnRep_CurrentAmmoInMagazine()
{
	OnAmmoAmountChange.Broadcast(CurrentAmmoInCarry, CurrentAmmoInMagazine);
}


/**************************************************************************/
/* FX */
/**************************************************************************/
void ATShootableWeapon::PlayWeaponFireVFX() const
{

	FVector MuzzleFlashLocation;
	FRotator MuzzleFlashRotation;

	if (FirePoint)
	{
		MuzzleFlashLocation = FirePoint->GetComponentLocation();
		MuzzleFlashRotation = FirePoint->GetComponentRotation();
		if (MuzzleFlash)
		{
			auto compNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlash, FirePoint, NAME_None, MuzzleFlashLocation, MuzzleFlashRotation, EAttachLocation::KeepWorldPosition, true);
			compNiagara->SetOnlyOwnerSee(false);
		}
		else if (MuzzleFlash_Particles)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash_Particles, FirePoint, NAME_None, MuzzleFlashLocation, MuzzleFlashRotation, EAttachLocation::KeepWorldPosition, true);
		}
	}
}


void ATShootableWeapon::PlayWeaponFireSFX() const
{
	if (FireSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSFX, GetActorLocation());
	}
}


void ATShootableWeapon::PlayWeaponReloadSFX() const
{
	if (MyPawn && MyPawn->IsLocallyControlled() && ReloadSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSFX, GetActorLocation());
	}
}


/**************************************************************************/
/* Replication */
/**************************************************************************/
void ATShootableWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATShootableWeapon, BurstFireCounter, COND_SkipOwner);
	DOREPLIFETIME(ATShootableWeapon, MyPawn);
	DOREPLIFETIME_CONDITION(ATShootableWeapon, CurrentAmmoInCarry, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATShootableWeapon, CurrentAmmoInMagazine, COND_OwnerOnly);

}
