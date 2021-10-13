// Fill out your copyright notice in the Description page of Project Settings.


#include "Turrets/TTurretBase.h"

// Engine Includes
#include "Components/SphereComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

#include "Perception/AIPerceptionComponent.h"
// Game Includes
#include "Player/TBasePlayerController.h"

// Sets default values
ATTurretBase::ATTurretBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereSensingComp = CreateDefaultSubobject<USphereComponent>(TEXT("SensingSphereComp"));
	if (SphereSensingComp)
	{
		SetRootComponent(SphereSensingComp);
		SphereSensingComp->SetSphereRadius(1000.f);
		SphereSensingComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereSensingComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SphereSensingComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECR_Overlap);
	}

	BaseMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComp"));
	if (BaseMeshComp)
	{
		BaseMeshComp->SetupAttachment(GetRootComponent());
	}

	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	if (FirePoint)
	{
		FirePoint->SetupAttachment(GetRootComponent());
	}
	
}

// Called when the game starts or when spawned
void ATTurretBase::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void ATTurretBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


APawn* ATTurretBase::LookForHostileTarget()
{
	//if (SphereSensingComp)
	//{
	//	TSet<AActor*> OverlappingActors;
	//	//TArray<APawn*> ValidTargets;
	//	SphereSensingComp->GetOverlappingActors(OverlappingActors, TrackingPawnClass);

	//	for (auto Actor : OverlappingActors)
	//	{
	//		APawn* OverlappingPawn = Cast<APawn>(Actor);
	//		if (OverlappingPawn)
	//		{
	//			auto TeamAgent = Cast<IGenericTeamAgentInterface>(OverlappingPawn->GetController());
	//			if (TeamAgent)
	//			{
	//				FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
	//				if (OtherTeamID.GetId() == TurretTeamID)
	//				{
	//					OverlappingActors.Remove(Actor);
	//					continue;
	//				}
	//			}

	//			if (!TargetWithinMaxAngle(Actor->GetActorLocation()))
	//			{
	//				OverlappingActors.Remove(Actor);
	//			}
	//		}
	//	}

	//	if (OverlappingActors.Num() > 0)
	//	{
	//		
	//		for (auto& Ele : OverlappingActors)
	//		{
	//			//auto TargetPawn = Cast<APawn>(Ele);
	//			CurrentTrackedPawn = Cast<APawn>(Ele);
	//			return CurrentTrackedPawn;
	//		}
	//	}
	//}

	//CurrentTrackedPawn = nullptr;
	//return nullptr;

	return CurrentTrackedPawn;
}


bool ATTurretBase::TargetWithinMaxAngle(const FVector& TargetLocation) const
{
	float YawLookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation).Yaw;
	float AngleToTarget = YawLookAtRotation - GetActorRotation().Yaw;
	if (AngleToTarget > 180.f)
	{
		AngleToTarget -= 360.f;
	}
	else if (AngleToTarget < -180.f)
	{
		AngleToTarget += 360.f;
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetAngle: %f"), AngleToTarget);

	return FMath::Abs(AngleToTarget) <= SensingAngle;
}

void ATTurretBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTurretBase, CurrentTrackedPawn);

}
