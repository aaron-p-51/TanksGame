// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/TPlayerTank.h"

// Engine Includes
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/PlayerState.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "AIController.h"
#include "NiagaraComponent.h"


// Game Includes
#include "Weapons/TShootableWeapon.h"
#include "../Components/THealthComponent.h"
#include "Player/TBasePlayerController.h"
#include "AI/TBaseAIController.h"
#include "Components/TBoostComponent.h"


ATPlayerTank::ATPlayerTank()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, SpringArmComp->SocketName);

	TurretMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMesh"));
	TurretMeshComp->SetupAttachment(GetRootComponent(), FName(TEXT("TowerSocket")));
	TurretMeshComp->SetIsReplicated(true);

	EngineAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TankEngineAudio"));
	EngineAudioComp->SetupAttachment(GetRootComponent());
	EngineAudioRPMParamName = TEXT("RPM");

	EngineExhauseNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TankExhaust"));
	EngineExhauseNiagaraComp->SetupAttachment(GetRootComponent());
	EngineExhaustSpawnRateParamName = TEXT("SpawnRate");

	HealthComp = CreateDefaultSubobject<UTHealthComponent>(TEXT("HealthComp"));

	BoostComp = CreateDefaultSubobject<UTBoostComponent>(TEXT("BoostComp"));

	bIsDead = false;

	bIsFlipping = false;
	MinRollToAllowFlip = 30.f;
	MinPitchToAllowFlip = 30.f;
	FlipImpulse = FVector(0.f, 0.f, 600.f);
	FlipCenterOfMass = FVector(0.f, 0.f, -100.f);
	FlipTime = 2.f;

	WeaponAttachSocketName = TEXT("BarrelSocket");

	CurrentSpringArmLengthIndex = 0;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}


void ATPlayerTank::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ATPlayerTank::SpawnDefaultInventory);
	}
}


void ATPlayerTank::BeginPlay()
{
	Super::BeginPlay();

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

	if (SpringArmComp)
	{
		if (SpringArmLengths.Num() >= 1)
		{
			SpringArmComp->TargetArmLength = SpringArmLengths[0];
		}
		else
		{
			SpringArmComp->TargetArmLength = 300.f;
		}
	}

	VehicleMovementComp = GetVehicleMovementComponent();
}


void ATPlayerTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsPlayerControlled() && GetLocalRole() == ENetRole::ROLE_Authority && bAILookAtTarget && AILookAtTarget)
	{
		AIRotateTurretToFaceTarget();
	}

	SetEngineAudioFromRotationSpeed();
	SetEngineExhaustFromRotationSpeed();
}


void ATPlayerTank::SetEngineAudioFromRotationSpeed()
{
	if (VehicleMovementComp && EngineAudioComp)
	{
		EngineAudioComp->SetFloatParameter(EngineAudioRPMParamName, VehicleMovementComp->GetEngineRotationSpeed());
	}
}


void ATPlayerTank::SetEngineExhaustFromRotationSpeed()
{
	if (VehicleMovementComp && EngineExhauseNiagaraComp)
	{
		float ExhaustSpawnRate = UKismetMathLibrary::MapRangeClamped(VehicleMovementComp->GetEngineRotationSpeed(), 0.f, VehicleMovementComp->GetEngineMaxRotationSpeed(), 50.f, 200.f);
		EngineExhauseNiagaraComp->SetFloatParameter(EngineExhaustSpawnRateParamName, ExhaustSpawnRate);
	}
}


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
	if (VehicleMovementComp)
	{	
		// Throttle to move forward
		if (Value > 0.f)
		{
			if (bInputApplyBoost && BoostComp && BoostComp->GetBoost() > 0.f)
			{
				VehicleMovementComp->SetTargetGear(2, true);
			}
			else
			{
				VehicleMovementComp->SetTargetGear(1, true);
			}
		}
		
		VehicleMovementComp->SetThrottleInput(Value);
		BP_SetThrottleTest(Value);
	}
}


void ATPlayerTank::MoveRight(float Value)
{
	if (VehicleMovementComp)
	{
		VehicleMovementComp->SetSteeringInput(Value);
	}
}


void ATPlayerTank::LookRight(float Value)
{
	if (SpringArmComp)
	{
		SpringArmComp->AddRelativeRotation(FRotator(0.f, Value, 0.f));
		FVector SpringArmRotation = SpringArmComp->GetRelativeRotation().Vector();
		ServerSetSpringArmRotation(SpringArmRotation);
		auto SpringArmCompOwner = SpringArmComp->GetOwner();
		if (SpringArmCompOwner && TurretMeshComp)
		{
			float YawValue = (SpringArmCompOwner->GetActorRotation().Yaw * -1.f) + SpringArmComp->GetTargetRotation().Yaw;
			TurretMeshComp->SetRelativeRotation(FRotator(0.f, YawValue, 0.f));
			ServerSetDesiredTurretYaw(YawValue);
		}
	}
}


void ATPlayerTank::ServerSetDesiredTurretYaw_Implementation(float YawValue)
{
	DesiredTurretYaw = YawValue;
	OnRep_SetDesiredTurretYaw();
}


void ATPlayerTank::OnRep_SetDesiredTurretYaw()
{
	if (TurretMeshComp)
	{
		TurretMeshComp->SetRelativeRotation(FRotator(0.f, DesiredTurretYaw, 0.f));
	}
}


void ATPlayerTank::LookUp(float Value)
{
	if (SpringArmComp)
	{
		SpringArmComp->AddRelativeRotation(FRotator(-Value, 0.f, 0.f));
	}
}


void ATPlayerTank::StartUseBoost()
{
	if (BoostComp)
	{
		BoostComp->StartUseBoost();
		bInputApplyBoost = true;
	}
}


void ATPlayerTank::StopUseBoost()
{
	if (BoostComp)
	{
		BoostComp->StopUseBoost();
		bInputApplyBoost = false;
	}

}


void ATPlayerTank::FlipUpright()
{
	// TODO: Fix FlipOver Implementation, replication issue with primitive component impulse and Center of mass offset
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("TODO: Fix FlipOver Implementation, replication issue with primitive component impulse and Center of mass offset"));
	}

	return;

	/*if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		ServerFlipUpright();
		return;
	}

	if (CanFlipOver() && GetMesh())
	{
		bIsFlipping = true;
		DefaultCenterOfMass = GetMesh()->GetSkeletalCenterOfMass();

		UE_LOG(LogTemp, Warning, TEXT("DefaultCenterOfMass %s"), *DefaultCenterOfMass.ToString());

		GetMesh()->AddImpulse(FlipImpulse, TEXT("None"), true);
		GetMesh()->SetCenterOfMass(FlipCenterOfMass);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_FlipOver, this, &ATPlayerTank::FlipTimerComplete, FlipTime);
	}*/
}


void ATPlayerTank::ServerFlipUpright_Implementation()
{
	FlipUpright();
}


bool ATPlayerTank::CanFlipOver() const
{
	if (bIsFlipping) return false;

	FRotator TankRotation = GetActorRotation();
	return FMath::Abs(TankRotation.Roll) >= MinRollToAllowFlip || FMath::Abs(TankRotation.Pitch) >= MinPitchToAllowFlip;
}


void ATPlayerTank::FlipTimerComplete()
{
	bIsFlipping = false;
	if (GetMesh())
	{
		GetMesh()->SetCenterOfMass(FVector(0.f, 0.f, -100.f));
	}
}


ATShootableWeapon* ATPlayerTank::GetCurrentWeapon() const
{
	return CurrentWeapon;
}


void ATPlayerTank::StartFire_Implementation()
{
	if (CurrentWeapon)
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


void ATPlayerTank::WeaponReload()
{
	if (CurrentWeapon)
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


void ATPlayerTank::OnHealthCompHealthChange(UTHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser)
{
	if (bIsDead) return;

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (FMath::IsNearlyZero(Health))
		{
			bIsDead = true;
		}

		// Inform Controller of health change
		InformControllerHealthChange(InstigatedBy, Health, HealthDelta, bIsDead);

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


void ATPlayerTank::OnBoostComponentBoostChange(float Boost, bool UsingBoost)
{
	UE_LOG(LogTemp, Warning, TEXT("Boost: %f"), Boost);

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

		BP_DebugBoostApplied(Boost, UsingBoost);
		
	}
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


void ATPlayerTank::InformControllerHealthChange(class AController* InstigatedBy, float NewHealth, float HealthDelta, bool IsDead)
{
	AController* MyController = GetController();
	if (MyController)
	{
		auto MyPlayerController = Cast<ATBasePlayerController>(MyController);
		if (MyPlayerController)
		{
			MyPlayerController->MyPawnHealthChange(InstigatedBy, NewHealth, HealthDelta, bIsDead);
		}
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


void ATPlayerTank::ServerSetSpringArmRotation_Implementation(FVector Value)
{
	if (SpringArmComp)
	{
		SpringArmComp->SetRelativeRotation(Value.Rotation());
	}
}


void ATPlayerTank::ChangeCameraView()
{
	if (SpringArmLengths.Num() > 1 && SpringArmComp)
	{
		CurrentSpringArmLengthIndex = (CurrentSpringArmLengthIndex + 1) % SpringArmLengths.Num();
		SpringArmComp->TargetArmLength = SpringArmLengths[CurrentSpringArmLengthIndex];
	}
}


void ATPlayerTank::OnRep_CurrentWeapon(ATShootableWeapon* LastWeapon)
{
	OnShootableWeaponChange.Broadcast(CurrentWeapon);
}


void ATPlayerTank::GetWeaponFirePoint(FVector& FirePointLocation, FRotator& FirePointRotation, EFirePoint FirePoint)
{
	// If AI controller is getting fire point it will always be from the BarrelSocket
	if (GetController<AAIController>())
	{
		FirePointRotation = TurretMeshComp->GetSocketRotation(TEXT("BarrelSocket"));
		FirePointLocation = TurretMeshComp->GetSocketLocation(TEXT("BarrelSocket"));
		return;
	}

	// For player controlled units
	if (FirePoint == EFirePoint::EFP_PlayerCamera && CameraComp)
	{
		FirePointLocation = CameraComp->GetComponentLocation();
		FirePointRotation = CameraComp->GetComponentRotation();
	}
	else if (FirePoint == EFirePoint::EFP_TurretMesh && TurretMeshComp)
	{
		FirePointLocation = TurretMeshComp->GetSocketLocation(TEXT("BarrelSocket"));

		// TODO: Change Turret up and down rotation (Animator)

		if (CameraComp)
		{
			FirePointRotation = CameraComp->GetComponentRotation();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ATPlayerTank::GetWeaponFirePoint - unable to get fire point, defaulted to actor location and rotation!!!"));
		FirePointLocation = GetActorLocation();
		FirePointRotation = GetActorRotation();
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


void ATPlayerTank::GiveHealth(float Amount)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && HealthComp)
	{
		HealthComp->AddHealth(Amount);
	}
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

	return 0.0f;
}


void ATPlayerTank::EnableAILookAtTarget(bool Enable, AActor* Target)
{
	// Verity pawn is begin controlled by an AI controller, and is running from the server
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AAIController* AIController = GetController<AAIController>();
		if (AIController)
		{
			bAILookAtTarget = Enable;
			AILookAtTarget = Target;
		}
	}
}


void ATPlayerTank::AIRotateTurretToFaceTarget()
{
	if (AILookAtTarget && TurretMeshComp)
	{
		float LookAtRotationYaw = UKismetMathLibrary::FindLookAtRotation(TurretMeshComp->GetComponentLocation(), AILookAtTarget->GetActorLocation()).Yaw;
		LookAtRotationYaw -= GetActorRotation().Yaw;
		TurretMeshComp->SetRelativeRotation(FRotator(0.f, LookAtRotationYaw, 0.f));
		DesiredTurretYaw = LookAtRotationYaw;
	}
}


/*************************************************************************/
/* Weapon Inventory */
/*************************************************************************/
void ATPlayerTank::SpawnDefaultInventory()
{
	// Weapons are replicated, only spawn on server
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

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
	if (Weapon)
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


void ATPlayerTank::SetCurrentWeapon(ATShootableWeapon* NewWeapon, ATShootableWeapon* PreviousWeapon)
{
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


	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		OnRep_CurrentWeapon(PreviousWeapon);
	}
}


void ATPlayerTank::SwitchToNextWeapon()
{
	// TODO: check if game input is allowed

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
		const int32 NextWeaponIndex = (CurrentWeaponIndex - 1) % WeaponInventory.Num();

		// TODO: Index can be out of range, causes crash occasionally. 
		if (NextWeaponIndex >= WeaponInventory.Num()) return;
		ATShootableWeapon* NextWeapon = WeaponInventory[NextWeaponIndex];

		if (!NextWeapon) return;
		EquipWeapon(NextWeapon);
	}
}


void ATPlayerTank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPlayerTank, DesiredTurretYaw);
	DOREPLIFETIME(ATPlayerTank, CurrentWeapon);
	DOREPLIFETIME(ATPlayerTank, bIsDead);
	DOREPLIFETIME_CONDITION(ATPlayerTank, WeaponInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATPlayerTank, bIsFlipping, COND_OwnerOnly);
}
