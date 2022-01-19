// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/TPlayerTank.h"


// Engine Includes
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "WheeledVehicleMovementComponent.h"


// Game Includes
#include "Components/TBoostComponent.h"
#include "../Components/THealthComponent.h"
#include "Player/TBasePlayerController.h"
#include "Weapons/TShootableWeapon.h"


static const float DEFAULT_SPRINGARM_LENGTH = 300.f;


ATPlayerTank::ATPlayerTank()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraGimbleComp = CreateDefaultSubobject<USceneComponent>(TEXT("CameraGimbleComp"));
	CameraGimbleComp->SetupAttachment(GetRootComponent());

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CameraGimbleComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, SpringArmComp->SocketName);
	

	TurretMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	TurretMeshComp->SetupAttachment(GetRootComponent(), FName(TEXT("TowerSocket")));
	

	EngineAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TankEngineAudio"));
	EngineAudioComp->SetupAttachment(GetRootComponent());
	EngineAudioRPMParamName = TEXT("RPM");

	EngineExhauseNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TankExhaust"));
	EngineExhauseNiagaraComp->SetupAttachment(GetRootComponent());
	EngineExhaustSpawnRateParamName = TEXT("SpawnRate");

	HealthComp = CreateDefaultSubobject<UTHealthComponent>(TEXT("HealthComp"));

	BoostComp = CreateDefaultSubobject<UTBoostComponent>(TEXT("BoostComp"));

	bIsDead = false;


	MaxCameraPitchAngle = 75.f;

	WeaponAttachSocketName = TEXT("BarrelSocket");

	CurrentSpringArmLengthIndex = 0;

	CurrentWeapon = nullptr;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}


/**************************************************************************/
/* Setup */
/**************************************************************************/
void ATPlayerTank::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ATPlayerTank::SpawnDefaultWeaponInventory);
	}
}


void ATPlayerTank::BeginPlay()
{
	Super::BeginPlay();

	BindToComponentEvents();
	SetInitialSpringArmLength();
}


void ATPlayerTank::BindToComponentEvents()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (HealthComp)
		{
			HealthComp->OnHealthChange.AddDynamic(this, &ATPlayerTank::OnHealthCompHealthChange);
		}

		if (BoostComp)
		{
			BoostComp->OnBoostChange.AddDynamic(this, &ATPlayerTank::OnBoostComponentBoostChange);
		}
	}
}


void ATPlayerTank::SetInitialSpringArmLength()
{
	if (SpringArmComp)
	{
		if (SpringArmLengths.Num() > 0)
		{
			SpringArmComp->TargetArmLength = SpringArmLengths[0];
		}
		else
		{
			SpringArmComp->TargetArmLength = DEFAULT_SPRINGARM_LENGTH;
		}
	}
}


void ATPlayerTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetEngineAudioFromRotationSpeed();
	SetEngineExhaustFromRotationSpeed();
	StabilizeCameraGimble();

	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		ServerSetCameraForWeaponFire(CameraComp->GetComponentLocation(), CameraComp->GetComponentRotation());
	}
}


void ATPlayerTank::StabilizeCameraGimble()
{
	if (CameraGimbleComp)
	{
		FRotator CameraGimbleRot(0.f, CameraGimbleComp->GetComponentRotation().Yaw, 0.f);
		CameraGimbleComp->SetWorldRotation(CameraGimbleRot);
	}
}


void ATPlayerTank::ServerSetCameraForWeaponFire_Implementation(FVector Pos, FRotator Rot)
{
	if (CameraComp)
	{
		CameraComp->SetWorldLocationAndRotation(Pos, Rot, false);
	}
}


ATShootableWeapon* ATPlayerTank::FindFirstWeaponUsesItem(EItemType ItemType) const
{
	for (const auto& Weapon : WeaponInventory)
	{
		if (Weapon->GetConsumableItem() == ItemType)
		{
			return Weapon;
		}
	}

	return nullptr;
}


/**************************************************************************/
/* Input */
/**************************************************************************/
void ATPlayerTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATPlayerTank::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATPlayerTank::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPlayerTank::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ATPlayerTank::LookRight);


	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ATPlayerTank::StartFire_Implementation);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ATPlayerTank::StopFire_Implementation);
	PlayerInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &ATPlayerTank::SwitchToNextWeapon);
	PlayerInputComponent->BindAction(TEXT("SwitchPreviousWeapon"), EInputEvent::IE_Pressed, this, &ATPlayerTank::SwitchToPreviousWeapon);

	PlayerInputComponent->BindAction(TEXT("FlipOver"), EInputEvent::IE_Pressed, this, &ATPlayerTank::FlipUpright);

	PlayerInputComponent->BindAction(TEXT("ChangeCameraView"), EInputEvent::IE_Pressed, this, &ATPlayerTank::ChangeCameraView);

	PlayerInputComponent->BindAction(TEXT("Boost"), EInputEvent::IE_Pressed, this, &ATPlayerTank::StartUseBoost);
	PlayerInputComponent->BindAction(TEXT("Boost"), EInputEvent::IE_Released, this, &ATPlayerTank::StopUseBoost);

	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &ATPlayerTank::WeaponReload);
}


void ATPlayerTank::MoveForward(float Value)
{
	if (!bIsDead)
	{
		GetVehicleMovementComponent()->SetThrottleInput(Value);
	}
}


void ATPlayerTank::MoveRight(float Value)
{
	if (!bIsDead)
	{
		GetVehicleMovementComponent()->SetSteeringInput(Value);
	}
}


void ATPlayerTank::LookRight(float Value)
{
	if (SpringArmComp && TurretMeshComp && !bIsDead )
	{
		SpringArmComp->AddRelativeRotation(FRotator(0.f, Value, 0.f));

		float SpringArmOwnerYaw = SpringArmComp->GetOwner()->GetActorRotation().Yaw * -1.f;
		float SprintArmTargetYaw = SpringArmComp->GetTargetRotation().Yaw;

		float NewTurretYaw = SpringArmOwnerYaw + SprintArmTargetYaw;
		TurretMeshComp->SetRelativeRotation(FRotator(0.f, NewTurretYaw, 0.f));

		if (GetLocalRole() == ENetRole::ROLE_Authority && IsLocallyControlled())
		{
			ServerSetTurretYaw_Implementation(NewTurretYaw);
		}
		else
		{
			ServerSetTurretYaw(NewTurretYaw);
		}
	}
}


void ATPlayerTank::LookUp(float Value)
{
	if (SpringArmComp && !bIsDead)
	{
		Value *= -1;

		// Check if adding pitch will exceed MaxCameraPitchAngle
		float NewDesiredPitch = SpringArmComp->GetComponentRotation().Pitch + Value;
		float NewDesiredRelativePitch = SpringArmComp->GetRelativeRotation().Pitch + Value;

		if (FMath::Abs(NewDesiredPitch) < FMath::Abs(MaxCameraPitchAngle) && FMath::Abs(NewDesiredRelativePitch) < FMath::Abs(MaxCameraPitchAngle))
		{
			SpringArmComp->AddRelativeRotation(FRotator(Value, 0.f, 0.f));
		}
	}
}


void ATPlayerTank::StartUseBoost()
{
	if (BoostComp && !bIsDead)
	{
		BoostComp->StartUseBoost();
		bInputApplyBoost = true;
	}
}


void ATPlayerTank::StopUseBoost()
{
	if (BoostComp && !bIsDead)
	{
		BoostComp->StopUseBoost();
		bInputApplyBoost = false;
	}
}


void ATPlayerTank::StartUseWeapon()
{
	StartFire_Implementation();
}


void ATPlayerTank::StopUseWeapon()
{
	StopFire_Implementation();
}


void ATPlayerTank::WeaponReload()
{
	Reload_Implementation();
}


void ATPlayerTank::ChangeCameraView()
{
	if (SpringArmLengths.Num() > 1 && SpringArmComp)
	{
		CurrentSpringArmLengthIndex = (CurrentSpringArmLengthIndex + 1) % SpringArmLengths.Num();
		SpringArmComp->TargetArmLength = SpringArmLengths[CurrentSpringArmLengthIndex];
	}
}


void ATPlayerTank::OnRep_PlayerSetTurretYaw()
{
	if (TurretMeshComp && !bIsDead)
	{
		TurretMeshComp->SetRelativeRotation(FRotator(0.f, PlayerSetTurretYaw, 0.f));
	}
}


void ATPlayerTank::ServerSetTurretYaw_Implementation(float Value)
{
	PlayerSetTurretYaw = Value;
	OnRep_PlayerSetTurretYaw();
}


void ATPlayerTank::FlipUpright()
{
	// TODO: Fix FlipOver Implementation, replication issue with primitive component impulse and Center of mass offset
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("TODO: Fix FlipOver Implementation, replication issue with primitive component impulse and Center of mass offset"));
	}

	return;
}


bool ATPlayerTank::PickupItem(EItemType ItemType, int32 Quantity)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && Quantity > 0 && ItemType != EItemType::EIT_NotSpecified && !bIsDead)
	{
		if (ItemType == EItemType::EIT_Health)
		{
			return GiveHealth((float)Quantity) > 0.f;
		}
		else if (ItemType == EItemType::EIT_MachineGunRounds || ItemType == EItemType::EIT_Rockets || ItemType == EItemType::EIT_HeavyShell)
		{
			ATShootableWeapon* Weapon = FindFirstWeaponUsesItem(ItemType);
			if (Weapon)
			{
				return Weapon->AddAmmo(Quantity) > 0;
			}
		}
	}

	return false;
}


void ATPlayerTank::SwitchToNextWeapon()
{
	// TODO: check if game input is allowed
	UE_LOG(LogTemp, Warning, TEXT("SwitchToNextWeapon"));

	if (WeaponInventory.Num() > 1)
	{
		const int32 CurrentWeaponIndex = WeaponInventory.IndexOfByKey(CurrentWeapon);
		const int32 NextWeaponIndex = (CurrentWeaponIndex + 1) % WeaponInventory.Num();

		ATShootableWeapon* NextWeapon = WeaponInventory[NextWeaponIndex];
		EquipWeapon(NextWeapon);
	}
}


void ATPlayerTank::SwitchToPreviousWeapon()
{
	// TODO: check if game input is allowed

	if (WeaponInventory.Num() > 1)
	{
		const int32 CurrentWeaponIndex = WeaponInventory.IndexOfByKey(CurrentWeapon);
		const int32 PreviousWeaponIndex = (CurrentWeaponIndex - 1 + WeaponInventory.Num()) % WeaponInventory.Num();

		ATShootableWeapon* PreviousWeapon = WeaponInventory[PreviousWeaponIndex];
		EquipWeapon(PreviousWeapon);
	}
}


/*************************************************************************/
/* State */
/*************************************************************************/
// TODO: Implement Flip
//bool ATPlayerTank::CanFlipOver() const
//{
//	if (bIsFlipping) return false;
//
//	FRotator TankRotation = GetActorRotation();
//	return FMath::Abs(TankRotation.Roll) >= MinRollToAllowFlip || FMath::Abs(TankRotation.Pitch) >= MinPitchToAllowFlip;
//}


float ATPlayerTank::GetEngineRotationSpeed() const
{
	return GetVehicleMovementComponent() ? FMath::Abs(GetVehicleMovementComponent()->GetEngineRotationSpeed()) : 0.f;
}


void ATPlayerTank::OnRep_IsDead()
{
	if (bIsDead)
	{
		PlayDeathFX();

		// Hide Tank Mesh
		auto TankBodyMesh = GetMesh();
		if (TankBodyMesh)
		{
			TankBodyMesh->SetVisibility(false, true);
		}
	}
}


void ATPlayerTank::GetShootableWeaponFirePoint(FVector& FireLocation, FRotator& FireRotation)
{
	// If AI controlled then fire point will always be from BarrelSocket on TurrentMeshComp
	if (!IsPlayerControlled() && TurretMeshComp)
	{
		FireLocation = TurretMeshComp->GetSocketLocation(TEXT("BarrelSocket"));
		FireRotation = TurretMeshComp->GetSocketRotation(TEXT("BarrelSocket"));
		return;
	}

	EShootableWeaponType CurrentWeaponType = GetEquipedWeaponType();

	if (CurrentWeaponType == EShootableWeaponType::ESWT_Instant && CameraComp)
	{
		FireLocation = CameraComp->GetComponentLocation();
		FireRotation = CameraComp->GetComponentRotation();
	}
	else if (CurrentWeaponType == EShootableWeaponType::ESWT_Projectile && TurretMeshComp && CameraComp)
	{
		FireLocation = TurretMeshComp->GetSocketLocation(TEXT("BarrelSocket"));
		FireRotation = CameraComp->GetComponentRotation();
	}
	else
	{
		FireLocation = GetActorLocation();
		FireRotation = GetActorRotation();
	}
}


/**************************************************************************/
/* Health */
/**************************************************************************/
float ATPlayerTank::GiveHealth(float Amount)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && HealthComp)
	{
		return HealthComp->AddHealth(Amount);
	}

	return 0.f;
}


float ATPlayerTank::GetMaxHealth() const
{
	if (HealthComp)
	{
		return HealthComp->GetDefaultHealth();
	}

	return 0.f;
}


float ATPlayerTank::GetHealth() const
{
	if (HealthComp)
	{
		return HealthComp->GetHealth();
	}

	return 0.f;
}


void ATPlayerTank::OnHealthCompHealthChange(float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && !bIsDead)
	{
		if (FMath::IsNearlyZero(Health))
		{
			bIsDead = true;
		}

		if (bIsDead)
		{
			auto TankMesh = GetMesh();
			if (TankMesh)
			{
				TankMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
			}

			PlayCameraShake(DeathCameraShake);

			OnRep_IsDead();

			SetLifeSpan(5.0f);
		}
		else
		{
			if (FMath::Abs(HealthDelta) >= BigHitMinDamage)
			{
				PlayCameraShake(BigHitCameraShake);
			}
		}
	}
}


/*************************************************************************/
/* FX */
/*************************************************************************/
void ATPlayerTank::PlayDeathFX()
{
	if (DeathExplosion)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathExplosion, GetActorLocation());
	}

	if (DeathSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSoundCue, GetActorLocation());
	}
}


void ATPlayerTank::PlayCameraShake(TSubclassOf<UCameraShake> CameraShakeClass)
{
	if (CameraShakeClass)
	{
		APlayerController* PC = GetController<APlayerController>();
		if (PC)
		{
			PC->ClientPlayCameraShake(CameraShakeClass);
		}
	}
}


void ATPlayerTank::SetEngineAudioFromRotationSpeed()
{
	if (EngineAudioComp)
	{
		EngineAudioComp->SetFloatParameter(EngineAudioRPMParamName, GetEngineRotationSpeed());
	}
}


void ATPlayerTank::SetEngineExhaustFromRotationSpeed()
{
	if (EngineExhauseNiagaraComp)
	{
		float EngineRotationSpeed = GetEngineRotationSpeed();
		float ExhaustSpawnRate = UKismetMathLibrary::MapRangeClamped(EngineRotationSpeed, 0.f, GetVehicleMovementComponent()->GetEngineMaxRotationSpeed(), 50.f, 200.f);
		EngineExhauseNiagaraComp->SetFloatParameter(EngineExhaustSpawnRateParamName, ExhaustSpawnRate);
	}
}


/**************************************************************************/
/* Boost */
/**************************************************************************/
float ATPlayerTank::GetBoost() const
{
	if (BoostComp)
	{
		return BoostComp->GetBoost();
	}

	return 0.f;
}


float ATPlayerTank::GetMaxBoost() const
{
	if (BoostComp)
	{
		return BoostComp->GetMaxBoost();
	}

	return 0.f;
}


void ATPlayerTank::OnBoostComponentBoostChange(float Boost, bool UsingBoost)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AController* MyController = GetController();
		if (MyController)
		{
			auto MyPlayerController = Cast<ATBasePlayerController>(MyController);
			if (MyPlayerController)
			{
				//MyPlayerController->MyPawnBoostChange(Boost);
			}
		}

		//BP_DebugBoostApplied(Boost, UsingBoost);
	}
}


/**************************************************************************/
/* Weapon */
/**************************************************************************/
void ATPlayerTank::StartFire_Implementation()
{
	if (CurrentWeapon && !bIsDead)
	{
		CurrentWeapon->StartFire();
	}
}


void ATPlayerTank::StopFire_Implementation()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}


bool ATPlayerTank::IsFiring_Implementation()
{
	if (CurrentWeapon)
	{
		return (CurrentWeapon->GetCurrentWeaponState() == EShootableWeaponState::ESWS_Idle) ? true : false;
	}

	return false;
}


void ATPlayerTank::OnRep_CurrentWeapon(ATShootableWeapon* LastWeapon)
{
	OnShootableWeaponChange.Broadcast(CurrentWeapon);
}


void ATPlayerTank::SpawnDefaultWeaponInventory()
{
	// Weapons are replicated, only spawn on server
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		WeaponInventory.Empty();
		for (int32 i = 0; i < WeaponInventoryClasses.Num(); ++i)
		{
			if (WeaponInventoryClasses[i])
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ATShootableWeapon* NewWeapon = GetWorld()->SpawnActor<ATShootableWeapon>(WeaponInventoryClasses[i], SpawnParams);
				AddWeapon(NewWeapon);
			}
		}

		if (WeaponInventory.Num() > 0)
		{
			EquipWeapon(WeaponInventory[0]);
		}
	}
}


void ATPlayerTank::AddWeapon(ATShootableWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ENetRole::ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		WeaponInventory.AddUnique(Weapon);
	}
}


void ATPlayerTank::EquipWeapon(ATShootableWeapon* Weapon)
{
	if (Weapon && WeaponInventory.Contains(Weapon) && !bIsDead)
	{
		if (GetLocalRole() < ENetRole::ROLE_Authority)
		{
			ServerEquipWeapon(Weapon);
		}
		else
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
	}
}


void ATPlayerTank::ServerEquipWeapon_Implementation(ATShootableWeapon* Weapon)
{
	EquipWeapon(Weapon);
}


void ATPlayerTank::Reload_Implementation()
{
	if (CurrentWeapon && !bIsDead)
	{
		CurrentWeapon->Reload();
	}
}


EShootableWeaponType ATPlayerTank::GetEquipedWeaponType() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponType();
	}

	return EShootableWeaponType::ESWT_DefaultMax;
}


void ATPlayerTank::SetCurrentWeapon(ATShootableWeapon* NewWeapon, ATShootableWeapon* PreviousWeapon)
{
	bool IsServer = GetLocalRole() == ENetRole::ROLE_Authority;

	UE_LOG(LogTemp, Warning, TEXT("ATPlayerTank::SetCurrentWeapon: Server %d"), IsServer);

	ATShootableWeapon* LocalPreviousWeapon = nullptr;

	if (PreviousWeapon != nullptr)
	{
		LocalPreviousWeapon = PreviousWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalPreviousWeapon = CurrentWeapon;
	}

	// TODO: unequip previous weapon

	CurrentWeapon = NewWeapon;


	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwningPawn(this);
		CurrentWeapon->OnEquip();
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		OnRep_CurrentWeapon(PreviousWeapon);
	}
}


/*************************************************************************/
/* Replication */
/*************************************************************************/
void ATPlayerTank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPlayerTank, CurrentWeapon);
	DOREPLIFETIME(ATPlayerTank, bIsDead);
	DOREPLIFETIME_CONDITION(ATPlayerTank, WeaponInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATPlayerTank, PlayerSetTurretYaw, COND_SkipOwner);
}
