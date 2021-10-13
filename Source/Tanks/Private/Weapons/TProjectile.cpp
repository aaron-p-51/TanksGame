// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TProjectile.h"

// Engine Includes
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"


// Sets default values
ATProjectile::ATProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	if (CollisionComp)
	{
		SetRootComponent(CollisionComp);
		CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
		CollisionRadius = CollisionComp->GetScaledSphereRadius();
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	if (MeshComp)
	{
		MeshComp->SetupAttachment(GetRootComponent());
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TrailEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffectComp"));
	if (TrailEffectComp && MeshComp)
	{
		TrailEffectComp->SetupAttachment(GetRootComponent());
	}

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileSFX = CreateDefaultSubobject<UAudioComponent>(TEXT("ProjectileSFX"));
	if (ProjectileSFX)
	{
		ProjectileSFX->SetupAttachment(GetRootComponent());
		ProjectileSFX->bAutoActivate = false;
	}

	DamageAmount = 30.f;
	DamageRadius = 300.f;
	bHitDetected = false;

	SetReplicates(true);
	
}


// Called when the game starts or when spawned
void ATProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Set Projectile to play the entire life of actor until collision is detected
	if (ProjectileSFX)
	{
		ProjectileSFX->Activate();
		ProjectileSFX->Play();
	}

	PreviousWorldLocation = GetActorLocation();
}


// Called every frame
void ATProjectile::Tick(float DeltaTime)
{
	if (bHitDetected) return;

	CurrentWorldLocation = GetActorLocation();
	FHitResult HitResult;
	if (DetectHit(HitResult))
	{
		bHitDetected = true;
		EmitOnHitEffects(HitResult.Location);

		if (GetLocalRole() == ENetRole::ROLE_Authority)
		{
			ApplyDamage(HitResult);
			TearOff();
		}

		HandleDestruction();
	}
	else
	{
		PreviousWorldLocation = CurrentWorldLocation;
	}
}


bool ATProjectile::DetectHit(FHitResult& HitResult)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	IgnoreActors.Add(GetInstigator());

	// Sphere trace from world position last frame to world position this frame
	bool HitDetected = UKismetSystemLibrary::SphereTraceSingle(
		this,
		PreviousWorldLocation,
		CurrentWorldLocation,
		CollisionRadius,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldDynamic),
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	return HitDetected;
}


void ATProjectile::EmitOnHitEffects(const FVector& Location) const
{
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Location);
	}

	if (ProjectileImpactSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileImpactSFX, Location);
	}
}


void ATProjectile::ApplyDamage(FHitResult& HitResult)
{
	TArray<AActor*> IgnoreActors;
	APawn* InstigatorPawn = GetInstigator();
	
	AController* Controller = nullptr;
	if (InstigatorPawn)
	{
		IgnoreActors.Add(Cast<AActor>(InstigatorPawn));
		Controller = InstigatorPawn->GetController();
	}

	// apply radial damage at the location collision was detected
	UGameplayStatics::ApplyRadialDamage(GetWorld(), DamageAmount, HitResult.Location, DamageRadius, DamageType, IgnoreActors, this, Controller);
}


void ATProjectile::HandleDestruction()
{
	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (MeshComp)
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// TrailEffectComp is child of mesh, DO NOT propagate to children
		MeshComp->SetVisibility(false, false);
	}

	BP_FinishDestroyProjectile();
}


void ATProjectile::SetLaunchVelocityVector(FVector LaunchVelocity, float GravityScale)
{
	// override ProjectileMovementComp values set in derived BP classes
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->Velocity = LaunchVelocity;
		ProjectileMovementComp->ProjectileGravityScale = GravityScale;
		ProjectileMovementComp->MaxSpeed = 0.f;
		ProjectileMovementComp->InitialSpeed = 0.f;
	}
}


void ATProjectile::SetLaunchSpeed(float LaunchSpeed)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->MaxSpeed = LaunchSpeed;
		ProjectileMovementComp->InitialSpeed = LaunchSpeed;
	}
}

