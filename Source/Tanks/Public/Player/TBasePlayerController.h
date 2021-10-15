// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDisplayCausedDamageInterface.h"
#include "GenericTeamAgentInterface.h"
#include "TMWGlobalEventHandler.h"
#include "GameFramework/PlayerController.h"
#include "TBasePlayerController.generated.h"

class UTPlayerTankHUD;
class UTMatchStartCountDownWidget;
class UTMultiplayerScoreBoard;
class ATShootableWeapon;
class UTPlayerTankMainWidget;

USTRUCT(BlueprintType)
struct FDisplayCausedDamage
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize DamageCausedLocation;

	UPROPERTY()
	float Damage;

	UPROPERTY()
	int32 DamageCausedCounter;
};

/**
 * 
 */
UCLASS()
class TANKS_API ATBasePlayerController : public APlayerController, public ITDisplayCausedDamageInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

/**
 * Members
 */

protected:


	/** Player HUD Widget class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UTPlayerTankMainWidget> PlayerTankMainWidgetClass;

	/** Players HUD */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UTPlayerTankMainWidget* PlayerHUD;

	/** Score Board Widget Class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MultiplayerScoreBoardWidgetClass;

	/** Score Board Widget */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UTMultiplayerScoreBoard* MultiplayerScoreBoard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bPawnControlEnabledOnStart;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_DamageDisplay)
	FDisplayCausedDamage DamageDisplay;

	UPROPERTY(Transient)
	int32 DamageDisplayCounter;

	

 /**
  * Methods
  */

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_DamageDisplay();

	/**
	 * [Server] Called when controller possess a pawn, will reset player HUD
	 * @param aPawn		New Pawn possessed by this controller
	 */
	virtual void OnPossess(APawn* aPawn) override;
	
	/** Reset Player HUD */
	UFUNCTION(Client, Reliable)
	void ClientResetPlayerHUD();

	/** [Client] Show MultiplayerScoreBoard Widget */
	void OnShowScoreBoard();

	/** [Client] Hide MultiplayerScoreBoard Widget */
	void OnHideScoreBoard();

	
	/**
	 * [Server] Inform GameMode of Damage, Score will be tallied in GameMode based on current rules in GameMode
	 * @param InstigatorController		Controller who caused damage to our pawn
	 * @param Damage					Amount of Damage caused to our pawn
	 * @param bMyPawnDied				Did our pawn die as a result of Damage
	 */
	void UpdateGameModeForPlayerDamage(AController* InstigatorController, float Damage, bool bMyPawnDied);

	/**
	 * [Client] Spawn the Floating Text Widget
	 * @param DamagedActor		Actor that was damaged
	 * @param Damage			Damage caused
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnDamageDisplayWidget(FVector_NetQuantize DisplayLocation, float Damage);


	UFUNCTION()
	void OnPlayerControllerDestroyed(AActor* Actor);


public:

	/**
	 * [Server] Handle our pawns health changing
	 * @param DamagingController	Controller who caused possessed pawns health to change
	 * @param CurrentHealth			Possessed pawns current health after health change (new health)
	 * @param HealthDelta			Change in health, negative values are damage, positive are heals
	 * @param bMyPawnDied			Did possessed pawn die as a result of health change
	 */
	void MyPawnHealthChange(AController* DamagingController, float CurrentHealth, float HealthDelta, bool bMyPawnDied);


	/** [Client] Bind Player Controller inputs */
	virtual void SetupInputComponent() override;

	/** [Client] Set if PlayerController can control possessed pawn */
	void SetEnablePlayerInputToPawn(bool bEnabled);

	UFUNCTION(Client, Reliable)
	void ClientWaitingForPlayers(int32 TimeToWait, const FString& Message);

	/** Start count down till match start  */
	UFUNCTION(Client, Reliable)
	void ClientStartMatchStartCountDownTimer(int32 CountDownSeconds, const FString& Message);

	/** Start count down for match (time when players will be playing) */
	UFUNCTION(Client, Reliable)
	void ClientStartMatchInProgress(int32 MatchDurationSeconds, const FString& Message);

	/** End match, clean up before travel (travel is not handled here) */
	UFUNCTION(Client, Reliable)
	void ClientEndMatchInProgress(int32 TimeLeftTillTravelSeconds, const FString& Message);



	/**
	 * [Server] ITDisplayCausedDamageInterface override, Display the Damaged caused to the DamagedActor. If
	 * player (Instigator of damage) is a client then ClientDisplayDamageWidget is used to show widget on client screen
	 * @param DamagedActor		The damaged actor, damage will show at this actors location
	 * @param Damage			Amount of damage
	 */
	virtual void DisplayDamageCaused(AActor* DamagedActor, float Damage) override;

	FTMWOnEventCalledSingle MyBoundEvent;

	UFUNCTION()
	void OnGameStateEventReceived(UObject* Publisher, UObject* Payload, const TArray<FString>& Metadata);


	/** Get this player GenericTeamID */
	FGenericTeamId GetGenericTeamId() const;


private:
	/** Team ID, used for AI to detect if hostile */
	FGenericTeamId TeamId;



};
