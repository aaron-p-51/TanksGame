// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TAIFireProjectile.generated.h"


class ATProjectile;

UENUM(BlueprintType)
enum class EBallisticMode : uint8
{
	/** Classic Ballistic calculation, will yield 0, 1 or 2 solutions, EPerferredBallisticArc determines what solution to use */
	EBM_NormalBallisticArc				UMETA(DisplayName = "NormalBallisticArc"),

	/** Calculated gravity scale to apply to projectile to achieve custom arc, projectile may fall faster then
	 * normal physics bodies influenced by gravity
	*/
	EBM_FixedLateralSpeedCustomArc		UMETA(DisplayName = "FixedLateralSpeedCustomArc")
};


UENUM(BlueprintType)
enum class EPerferredBallisticArc : uint8
{
	EPBA_LowAngleSolution				UMETA(DisplayName = "LowSolutionAngle"),
	EPBA_HighAngleSolution				UMETA(DisplayName = "HighSolutionAngle"),
	EPBA_AnySolutionLowBias				UMETA(DisplayName = "AnySolutionLowBias"),
	EPBA_AnySolutionHighBias			UMETA(DisplayName = "AnySolutionHighBias"),
	EPBA_AnySolution					UMETA(DisplayName = "AnySolution")
};


UENUM(BlueprintType)
enum class ESelectedBallisticArc : uint8
{
	ESBA_LowAngleSolution				UMETA(DisplayName = "LowAngleSolution"),
	ESBA_HighAngleSolution				UMETA(DisplayName = "HighAngleSolution"),
	ESBA_NoSolutionSelected				UMETA(DisplayName = "NoSolutionSelected")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKS_API UTAIFireProjectile : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */
private:

	/** World gravity (down positive) it is assumed gravity world gravity will not change during gameplay */
	float WorldGravity;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<ATProjectile> ProjectileClass;

	/**
	 * Calculation methods to calculate ballistic trajectory. See EBallisticMode definition
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BallisticConfig")
	EBallisticMode BallisticMode;

	/** 
	 * Minimum launch acceptable for ballistic solution. If calculated launch angle is less than MinVerticalLaunchAngle
	 * projectile will not be fired 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BallisticConfig")
	float MinVerticalLaunchAngle;

	/**
	 * Maximum launch acceptable for ballistic solution. If calculated launch angle is greater than MaxVerticalLaunchAngle
	 * projectile will not be fired
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BallisticConfig")
	float MaxVerticalLaunchAngle;

	/** Scalar velocity to launch projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BallisticConfig", meta = (ClampMin=0.f))
	float ProjectileVelocity;

	/** Use a random offset in ballistic calculations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BallisticConfig|Target")
	bool bUseRandomTargetOffset;

	/** Distance from Target to apply random offset, direction will be random */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BallisticConfig|Target", meta = (EditCondition = "bUseRandomTargetOffset"))
	float RandomTargetOffsetRadius;

	/** Projectiles Gravity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NormalBallisticArcConfig")
	float ProjectileGravity;

	/** Preferred Ballistic Arc if multiple ballistic solutions are found */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NormalBallisticArcConfig")
	EPerferredBallisticArc PerferredBallisticArc;

	/** Target Arc height the projectile should reach */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FixedLateralSpeedCustomArcConfig", meta = (ClampMin=0.f))
	float ArcPeak;


 /**
  * Methods
  */


protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	
	/**
	 * [Server] Fire projectile at target using classic ballsitic equation. Will solve and launch based on members above
	 * @param FirePointLocation	- Point where projectile will be launched from
	 * @param TargetLocation	- Point where target is located
	 * @param TargetVelocity	- Target velocity at time of launch
	 */
	void FireProjectileNormalArc(FVector FirePointLocation, FVector TargetLocation, FVector TargetVelocity);

	/**
	 * [Server] Fire projectile at target using custom arc settings. Used to get desired projectile arc. Projectile gravity will be
	 * adjusted for desired arc
	 * @param FirePointLocation	- Point where projectile will be launched from
	 * @param TargetLocation	- Point where target is located
	 * @param TargetVelocity	- Target velocity at time of launch
	 */
	void FireProjectileCustomArc(FVector FirePointLocation, FVector TargetLocation, FVector TargetVelocity);

	/** Apply a random offset to targets location to calculate ballistic path*/
	void ApplyRandomTargetOffset(FVector& TargetPosition);

	/** Select what solution to use if multiply ballistic solutions are found, only used for EBM_NormalBallisticArc  */
	ESelectedBallisticArc SelectDesiredNormalArcSolution(int32 ValidSolutions, bool bLowAngleSolutionValid, bool bHighAngleSolutionValid);

	/**
	 * [Client + Server] Spawn and launch projectile for all clients and server. Projectile does not replicate movement.
	 * @param SpawnLocation		- Location where projectile is spawned
	 * @param LaunchVelocity	- Launch Velocity of projectile
	 * @param Gravity			- Gravity scale to apply to projectile, will be World Gravity if called from FireProjectileNormalArc().
	 *							  FireProjectileCustomArc() will change gravity scale of projectile
	 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnAndLaunchProjectile(FVector SpawnLocation, FVector LaunchVelocity, float Gravity);

	/**
	 * Determine if the calculated launch angle is within angle: TODO finish method
	 */
	bool IsWithinLaunchAngle(const FVector& LaunchPoint, const FVector& LaunchVelocity) const;


public:

	// Sets default values for this component's properties
	UTAIFireProjectile();

	UFUNCTION(BlueprintCallable)
	void FireProjectile(FVector FirePointLocation, AActor* Target);

	UFUNCTION(BlueprintImplementableEvent, Category = "LaunchVelocity")
	void WithinFireAngleConstraints(FVector FireVelocity, bool& Result);

};
