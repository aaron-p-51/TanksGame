// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TBallisticsCalculator.generated.h"


USTRUCT(BlueprintType)
struct FBallisticArcSolution
{
	GENERATED_BODY()

public:
	FVector LowAngleFireVelocity;
	FVector HighAngleFireVelocity;

};


USTRUCT(BlueprintType)
struct FBallisticArcLateralSolution
{
	GENERATED_BODY()

public:
	FVector FireVelocity;
	float Gravity;

};


USTRUCT(BlueprintType)
struct FBallisticArcLateralMovingTargetSolution
{
	GENERATED_BODY()

public:
	FVector FireVelocity;
	float Gravity;
	FVector ImpactPoint;

};


/**
 * 
 */
UCLASS()
class TANKS_API UTBallisticsCalculator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Solve for Cubic Root of Value
	*	Return cubic root of Value
	*/
	static double GetCubicRoot(double Value);


	/**
	 *	Solve quadratic equation: c0*x^2 + c1*x + c2.
	 *	Returns number of solutions.
	 */
	static int32 SolveQuadric(double C0, double C1, double C2, double& S0, double& S1);


	/**
	 * Solve cubic equation: c0*x^3 + c1*x^2 + c2*x + c3.
	 * Returns number of solutions.
	 */
	static int32 SolveCubic(double C0, double C1, double C2, double C3, double& S0, double& S1, double& S2);


	/**
	 * Solve quartic function: c0*x^4 + c1*x^3 + c2*x^2 + c3*x + c4.
	 * Returns number of solutions.
	 */
	static int32 SolveQuartic(double C0, double C1, double C2, double C3, double C4, double& S0, double& S1, double& S2, double& S3);


	/**
	 * Calculate the maximum range that a ballistic projectile can be fired on given speed and gravity.
	 *
	 * @Speed:				projectile velocity
	 * @Gravity:			force of gravity, positive is down
	 * @InitialHeight:		 distance above flat terrain
	 *
	 * return (float): maximum range
	 */
	UFUNCTION(BlueprintCallable)
	static float BallisticRange(float Speed, float Gravity, float InitialHeight);


	/**
	 * // Solve firing angles for a ballistic projectile with speed and gravity to hit a fixed position.
	 *
	 * ProjectilePos:		point projectile will fire from
	 * ProjectileSpeed:		scalar speed of projectile
	 * TargetPos:			point projectile is trying to hit
	 * Gravity:				force of gravity, positive down
	 *
	 * @Solution
	 *		Firing solution, high and low angles, if valid
	 *
	 * return: number of unique solutions found: 0, 1, or 2.
	 */
	UFUNCTION(BlueprintCallable)
	static int32 SolveBallisticArc(FVector ProjectilePos, float ProjectileSpeed, FVector TargetPos, float Gravity, FBallisticArcSolution& Solution);


	/**
	 *
	 * Solve firing angles for a ballistic projectile with speed and gravity to hit a target moving with constant, linear velocity.
	 *
	 * @ProjectilePos:		point projectile will fire from
	 * @ProjectileSpeed:	scalar speed of projectile
	 * @TargetPos:			point projectile is trying to hit
	 * @TargetVelocity:		velocity of target
	 * @Gravity:			force of gravity, positive down
	 *
	 * @Solution
	 *		Firing solution, high and low angles, if valid
	 *
	 * return: number of unique solutions found: 0, 1, 2
	 */
	UFUNCTION(BlueprintCallable)
	static int32 SolveBallisticArcMovingTarget(FVector ProjectilePos, float ProjectileSpeed, FVector TargetPos, FVector TargetVelocity, float Gravity, FBallisticArcSolution& Solution);


	/**
	 * Solve the firing arc with a fixed lateral speed. Vertical speed and gravity varies.
	 * This enables a visually pleasing arc.
	 *
	 * @ProjectilePos:		point projectile will fire from
	 * @LateralSpeed:		scalar speed of projectile along XY plane
	 * @TargetPos:			point projectile is trying to hit
	 * @max_height:			height above Max(ProjectilePos, FBallisticArcLateralMovingTargetSolution.ImpactPos) for projectile to peak at
	 *
	 * @Solution:
	 *		FireVelocity: firing velocity
	 *		Gravity: gravity necessary to projectile to hit precisely MaxHeightOffset
	 *
	 * return: true if a valid solution was found
	 */
	UFUNCTION(BlueprintCallable)
	static bool SolveBallisticArcLateral(FVector ProjectilePos, float LateralSpeed, FVector TargetPos, float MaxHeightOffset, FBallisticArcLateralSolution& Solution);


	/**
	 * Solve the firing arc with a fixed lateral speed. Vertical speed and gravity varies.
	 * This enables a visually pleasing arc.
	 *
	 * @ProjectilePos:		point projectile will fire from
	 * @LateralSpeed:		scalar speed of projectile along XY plane
	 * @TargetPos:			point projectile is trying to hit
	 * @TargetVelocity:		velocity of target
	 * @MaxHeightOffset:	height above Max(ProjectilePos, FBallisticArcLateralMovingTargetSolution.ImpactPos) for projectile to peak at
	 *
	 * @Solution:
	 *		FireVelocity: firing velocity
	 *		Gravity: gravity necessary to projectile to hit precisely MaxHeightOffset
	 *		ImpactPoint: point where moving target will be hit
	 *
	 * return (bool): true if a valid solution was found
	 */
	UFUNCTION(BlueprintCallable)
	static bool SolveBallisticArcLateralMovingTarget(FVector ProjectilePos, float LateralSpeed, FVector TargetPos, FVector TargetVelocity, float MaxHeightOffset, FBallisticArcLateralMovingTargetSolution& Solution);

};
