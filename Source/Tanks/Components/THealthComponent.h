// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "THealthComponent.generated.h"

// Delegate for when Health changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangeSignature, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, HealthChangeCauser);


/**
 * Health Component
 */
UCLASS( ClassGroup=(Tanks), meta=(BlueprintSpawnableComponent) )
class TANKS_API UTHealthComponent : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */
private:
	
	/**************************************************************************/
	/* Configuration */
	/**************************************************************************/

	/** Health Value that should be treated as base health value. Can be used as Max health value for Actors with this component */
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	float DefaultHealth;


	/**************************************************************************/
	/* State */
	/**************************************************************************/

	/** Controls if Actor with this component can be damaged. */
	UPROPERTY(EditDefaultsOnly, Category = "State")
	bool bCanBeDamaged;

	/** Controls if Actor with this component will reject all damage. Should be used for debug and testing */
	UPROPERTY(EditAnywhere, Category = "State")
	bool bIsInvincible;

	/** Current Health value, valid values will be between 0.f and DefaultHealth */
	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category = "State")
	float Health;


/**
 * Methods
 */
private:

	/** [Server] Bound to owning Actor OnTakeAnyDamage event. Apply reduction to Health and broadcast OnHealthChange event */
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** Broadcast OnHealthChange to clients */
	UFUNCTION()
	void OnRep_Health();

	/* [Server] Check if DamageCausingController implements ITDisplayCausedDamageInterface if so call DisplayDamageCaused */
	void AlertDamageToDisplayCausedDamageInterfaceController(AController* DamageCausingController, AActor* DamagedActor, float DamageAmount) const;


protected:

	// Called when the game starts
	virtual void BeginPlay() override;


public:

	// Sets default values for this component's properties
	UTHealthComponent();

	/** Event when Health member changes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangeSignature OnHealthChange;

	/**
	 * [Server] Add Amount to Health, will not set Health greater than DefaultHealth. Return value
	 * will be actual amount of health added. Returns 0 if called from client.
	 */
	float AddHealth(float Amount);

	/** [Server] Prevent damage and Health from decreasing. Will still add health via AddHealth() method */
	void SetCanBeDamaged(bool CanBeDamaged);

	/** Accessors for private members */
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetDefaultHealth() const { return DefaultHealth; }

	

};
