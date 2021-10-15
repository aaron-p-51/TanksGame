// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTanksTypes.h"
#include "TMWGlobalEventHandler.h"
#include "Components/ActorComponent.h"
#include "THealthComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, UTHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, HealthChangeCauser);



UCLASS( ClassGroup=(Tanks), meta=(BlueprintSpawnableComponent) )
class TANKS_API UTHealthComponent : public UActorComponent
{
	GENERATED_BODY()


/**
 * Members
 */
private:
	
	bool bCanBeDamaged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	bool bIsInvincible;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
	TArray<EGameInProgressState> ApplyDamageInGameStates;

/**
 * Methods
 */
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	// Sets default values for this component's properties
	UTHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangeSignature OnHealthChange;

	void AddHealth(float Amount);

	FORCEINLINE float GetHealth() const { return Health; }

	FORCEINLINE float GetDefaultHealth() const { return DefaultHealth; }

	FTMWOnEventCalledSingle BoundGameStateChangedEvent;

	void AddApplyDamageInGameState(EGameInProgressState State);

	UFUNCTION()
	void OnGameStateEventReceived(UObject* Publisher, UObject* Payload, const TArray<FString>& Metadata);

};
