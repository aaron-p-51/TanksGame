// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TTeamAIController.h"

#include "Perception/AIPerceptionComponent.h"

ATTeamAIController::ATTeamAIController()
{
	TeamNumber = 5;
	TeamID = FGenericTeamId(TeamNumber);
	//SetGenericTeamId(FGenericTeamId(TeamNumber));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPercenptionComp");
}


ETeamAttitude::Type ATTeamAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	int32 i = 0;
	i++;
	UE_LOG(LogTemp, Warning, TEXT("I is: %d"), i);

	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			//return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
			if (OtherTeamID == TeamNumber)
			{
				return ETeamAttitude::Friendly;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}

	}

	return ETeamAttitude::Neutral;
}

FGenericTeamId ATTeamAIController::GetGenericTeamId() const
{
	return TeamID;
}

void ATTeamAIController::SetTeamNumber(uint8 Number)
{
	TeamNumber = Number;
	TeamID = FGenericTeamId(TeamNumber);
	//SetGenericTeamId(FGenericTeamId(TeamNumber));
}

void ATTeamAIController::GetTargetActors(TArray<AActor*>& TargetActors)
{
	if (AIPerceptionComponent)
	{
		//TArray<AActor*> Targets;
		TargetActors.Empty();
		AIPerceptionComponent->GetHostileActors(TargetActors);
	}
}
