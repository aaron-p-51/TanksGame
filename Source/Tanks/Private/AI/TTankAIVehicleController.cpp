// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TTankAIVehicleController.h"

//#include "Perception/AIPerceptionComponent.h"
#include "Components/VehiclePathFollowingComponent.h"


ATTankAIVehicleController::ATTankAIVehicleController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UVehiclePathFollowingComponent>(TEXT("PathFollowingComponent")))
{

	/*AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPercenptionComp");


	TeamNumber = 35;
	TeamID = FGenericTeamId(TeamNumber);*/
}


FVector ATTankAIVehicleController::OnNewPathSegment_Implementation(int PathPointIndex, FVector StartSegmentLocation, FVector TargetSegmentLocation, const UClass* AreaClass, bool bIsOnNavLink)
{
	return TargetSegmentLocation;
}


void ATTankAIVehicleController::FireAtTarget(float Duration, float DurationVariance)
{
	UE_LOG(LogTemp, Warning, TEXT("Tank FireAtTarget"));
}

FVector ATTankAIVehicleController::GetCustomDestination_Implementation(FVector CurrentMoveFocus)
{
	return CurrentMoveFocus;
}


//ETeamAttitude::Type ATTankAIVehicleController::GetTeamAttitudeTowards(const AActor& Other) const
//{
//	if (const APawn* OtherPawn = Cast<APawn>(&Other))
//	{
//		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
//		{
//			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
//			if (OtherTeamID == TeamNumber)
//			{
//				return ETeamAttitude::Friendly;
//			}
//			else
//			{
//				return ETeamAttitude::Hostile;
//			}
//		}
//	}
//	
//	return ETeamAttitude::Neutral;
//}
//
//FGenericTeamId ATTankAIVehicleController::GetGenericTeamId() const
//{
//	return TeamID;
//}

