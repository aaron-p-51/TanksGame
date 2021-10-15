// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TBaseAIController.h"
//#include "GenericTeamAgentInterface.h"
#include "TTankAIVehicleController.generated.h"


//class UAIPerceptionComponent;

/**
 * 
 */
UCLASS()
class TANKS_API ATTankAIVehicleController : public ATBaseAIController //AAIController
{
	GENERATED_BODY()

/**
 * Members
 */

// public:
//	
//	 UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
//	 AActor* CurrentCombatTarget;
//
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TeamID")
//	uint8 TeamNumber;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TeamID")
//	FGenericTeamId TeamID;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
//	UAIPerceptionComponent* AIPerceptionComponent;

	


 /**
  * Methods
  */
public:

	ATTankAIVehicleController(const FObjectInitializer& ObjectInitializer);


	/**
	* Change the current move destination than the one the pathfinding system is providing (In debug mode, this is a blue square).
	* Useful for following a path precisely every frame, like a spline to navigate a difficult corner or obstacle.
	* @param CurrentMoveFocus - The target that the pathfinding system has set, and the agent is currently moving towards
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Custom Path Control")
	FVector GetCustomDestination(FVector CurrentMoveFocus);

	/**
	* Allows you to change the current path point to a different location (Debug mode: Yellow square).
	* This is executed once every time the vehicle reaches a new path point, so can be more optimial than GetCustomDestination (executed every frame)
	* @param PathPointIndex - The index of this new path point, which can then be used to check other path points relative to this one
	* @param StartSegmentLocation - The point which the vehicle reached when it triggered this event
	* @param TargetSegmentLocation - The unchanged path point this vehicle is trying to reach
	* @param AreaClass - The area class of the target segment
	* @param bIsOnNavLink - Whether or not the target segment is on a nav link or not
	* @return The new location for this path point to be set
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Custom Path Control")
	FVector OnNewPathSegment(int PathPointIndex, FVector StartSegmentLocation, FVector TargetSegmentLocation, const UClass* AreaClass, bool bIsOnNavLink);

	//ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	///** Get this player GenericTeamID */
	//FGenericTeamId GetGenericTeamId() const;

	
	virtual void FireAtTarget(float Duration, float DurationVariance) override;
};
