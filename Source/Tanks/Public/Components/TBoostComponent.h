// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TBoostComponent.generated.h"

UENUM(BlueprintType)
enum class EBoostState : uint8
{
	EBS_Charged			UMETA(DisplayName = "Charged"),
	EBS_Using			UMETA(DisplayName = "Using"),
	EBS_RechargeDelay	UMETA(DisplayName = "RechargeDelay"),
	EBA_Recharging		UMETA(DisplayName = "Automatic")
};

// On Boost change event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoostChangeSignature, float, Boost, bool, UsingBoost);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKS_API UTBoostComponent : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */
protected:

	/** Current Boost available */
	UPROPERTY(ReplicatedUsing=OnRep_Boost, VisibleAnywhere, BlueprintReadWrite, Category = "BoostConfiguration")
	float Boost;

	/** Boost level boost will be recharged to */
	UPROPERTY(EditDefaultsOnly, Category = "BoostConfiguration")
	float MaxBoost;

	/** When using boost how fast is it used (Boost/sec) */
	UPROPERTY(EditDefaultsOnly, Category = "BoostConfiguration")
	float UseRate;

	/** When recharging boost how fast does it recharge (Boost/sec) */
	UPROPERTY(EditAnywhere, Category = "BoostConfiguration")
	float RechargeRate;

	/** After using boost how long until it starts recharging */
	UPROPERTY(EditAnywhere, Category = "BoostConfiguration")
	float RechargeDelay;

	/** Current Boost State */
	EBoostState BoostState;

	FTimerHandle RechargeDelayTimerHandle;

public:

	/** Delegate to bind to, is fired when Boost changes  */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBoostChangeSignature OnBoostChange;

 /**
  * Methods
  */
public:	
	// Sets default values for this component's properties
	UTBoostComponent();

	/** [Server] Start using boost will start depleting Boost until either Boost is 0 or StopUseBoost() is called */
	void StartUseBoost();

	UFUNCTION(Server, Reliable)
	void ServerStartUseBoost();

	/** [Server] Stop using boost, will trigger timer for recharge delay */
	void StopUseBoost();

	UFUNCTION(Server, Reliable)
	void ServerStopUseBoost();

	/** Get the current amount of Boost */
	FORCEINLINE float GetBoost() const { return Boost; }

	/** Get the max amount of boost possible */
	FORCEINLINE float GetMaxBoost() const { return MaxBoost; }

	

protected:


	/** [Server] Change BoostState to EBA_Recharging after RechargeDelay is complete */
	void RechargeDelayComplete();

	/** Brodcasts OnBoostChange delegate */
	UFUNCTION()
	void OnRep_Boost();

	/** [Server] Updates current boost level based on BoostState */
	void UpdateBoostLevel(float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
