// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDisplayCausedDamageInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "TBasePlayerController.generated.h"


/** Forward declaractions */
class UTPlayerTankHUD;
class UTMatchStartCountDownWidget;
class UTMultiplayerScoreBoard;
class ATShootableWeapon;
class UTPlayerTankMainWidget;
class ATPlayerTank;

/** Display damage caused by this player */
USTRUCT(BlueprintType)
struct FDisplayCausedDamage
{
	GENERATED_BODY()

	/** Location in world to show damage amount */
	UPROPERTY()
	FVector_NetQuantize DamageCausedLocation;

	/** Amount of damage caused */
	UPROPERTY()
	float Damage;

	/** Counter for damage caused, forces replication */
	UPROPERTY()
	int32 DamageCausedCounter;

};


/**
 * Base class for Tanks PlayerController
 */
UCLASS()
class TANKS_API ATBasePlayerController : public APlayerController, public ITDisplayCausedDamageInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

/**
 * Members
 */

private:

	/**************************************************************************/
	/* Config */
	/**************************************************************************/

	/** Player HUD Widget class */
	UPROPERTY(EditDefaultsOnly, Category = "Config|Widgets")
	TSubclassOf<UTPlayerTankMainWidget> PlayerTankMainWidgetClass;

	/** Players HUD */
	UPROPERTY(VisibleDefaultsOnly, Category = "Config|Widgets")
	UTPlayerTankMainWidget* PlayerTankMainWidget;

	/** Score Board Widget Class */
	UPROPERTY(EditDefaultsOnly, Category = "Config|Widgets")
	TSubclassOf<UUserWidget> MultiplayerScoreBoardWidgetClass;

	/** Score Board Widget */
	UPROPERTY(VisibleDefaultsOnly, Category = "Config|Widgets")
	UTMultiplayerScoreBoard* MultiplayerScoreBoard;

	/** Default Pawn control. Set to false for gamestate to set according to selected game mode  */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bPawnControlEnabledOnStart;


	/**************************************************************************/
	/* State */
	/**************************************************************************/

	/** Team ID, used for friendly enemy detection */
	UPROPERTY(VisibleAnywhere)
	FGenericTeamId TeamId;

	/** Cached reference to controlled PlayerTank pawn */
	UPROPERTY(VisibleAnywhere)
	ATPlayerTank* MyPlayerTank;

	/** Display damage in world caused by this player. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_DamageDisplay)
	FDisplayCausedDamage DamageDisplay;

	/** Enable owning control to owning pawn */
	UPROPERTY(ReplicatedUsing = OnRep_EnablePawnControl)
	uint8 bEnablePawnControl : 1;


 /**
  * Methods
  */

public:

	ATBasePlayerController();


	/**************************************************************************/
	/* Setup */
	/**************************************************************************/

protected:

	virtual void BeginPlay() override;

private:

	/** [Server + Client] Bind to relevant GameInProgressState changes */
	void BindToGameStateStateChangeEvents();

	/**
	 * [Server] Bound to ATMultiplayerBaseGameState OnGameInProgress event. Enable damage for player Pawn. 
	 *	Enable pawn control for owning pawn.
	 */
	UFUNCTION()
	void OnGameInProgressStateChange(int32 StateTimeSeconds);

	/**
	 * [Server + Client] Bound to ATMultiplayerBaseGameState OnGameExitCountDown event. Disable pawn control on server
	 * Remove PlayerTankMainWidget from viewport and show MultiplayerScoreBoard
	 */
	UFUNCTION()
	void OnGameExitCountdownStateChange(int32 StateTimeSeconds);

	/** [Client] Create and add instance of PlayerTankMainWidgetClass to calling players viewport */
	void CreatePlayerHUDWidget();

	/** [Client] Set PlayerTankMainWidget for new pawn. Called when new pawn is assigned to this PlayerController  */
	void ResetPlayerTankMainWidget();

	/** [Client] Create and add instance of MultiplayerScoreBoardWidgetClass to calling player's viewport  */
	void CreateScoreWidget();

	/** [Server] Bind to events send from MyPlayerTank and MyPlayerTank owned components */
	void BindPlayerTankPawnEvents();


	/**************************************************************************/
	/* Input */
	/**************************************************************************/

public:

	/** [Client] Bind Player Controller inputs */
	virtual void SetupInputComponent() override;

private:

	/** [Client] Show MultiplayerScoreBoard Widget */
	void OnShowScoreBoard();

	/** [Client] Hide MultiplayerScoreBoard Widget */
	void OnHideScoreBoard();


	/**************************************************************************/
	/* Damage (Health)  */
	/**************************************************************************/

	/**
	 * [Server] Handle pawns health changing. Bound to MyPlayerTank HealthComp
	 * @param Health				New Health
	 * @param HealthDelta			Change in Health value
	 * @param DamageType			Type of damage that caused health change
	 * @param InstigatedBy			Controller who caused Health to change.
	 * @param HealthChangeCauser	Actor who caused Health to change.
	 */
	UFUNCTION()
	void MyPlayerTankHealthChange(float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser);

	/**
	 * [Server] Inform GameMode of Damage, Score will be tallied in GameMode based on current rules in GameMode
	 * @param InstigatorController		Controller who caused damage to our pawn
	 * @param Damage					Amount of Damage caused to our pawn
	 * @param bMyPawnDied				Did our pawn die as a result of Damage
	 */
	void UpdateGameModeForPlayerDamage(AController* InstigatorController, float Damage, bool bMyPawnDied);

	/** [Server + Client] Get the player name for KillerController. Player Name will be set PlayerName in KillerController's PlayerState */
	FString GetKilledByPlayerName(AController* KillerController) const;

	/** [Client] Inform client it's pawn died. Alert PlayerTankMainWidget */
	UFUNCTION(Client, Reliable)
	void ClientControlledPawnDied(const FString& KillerPlayerName);

	/** [Server] Set MyPlayerTank ability to be de damaged */
	void SetControlledPawnCanTakeDamage(bool CanTakeDamage);

	UFUNCTION()
	void OnRep_DamageDisplay();

public:

	/**
	 * [Server] ITDisplayCausedDamageInterface override, Display the Damaged caused to the DamagedActor.
	 *
	 * @param DamagedActor		The damaged actor, damage will show at this actors location
	 * @param Damage			Amount of damage
	 */
	virtual void DisplayDamageCaused(AActor* DamagedActor, float Damage) override;

protected:

	/**
	 * [Client] Spawn the Floating Text Widget
	 * @param DamagedActor		Actor that was damaged
	 * @param Damage			Damage caused
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnDamageDisplayWidget(FVector_NetQuantize DisplayLocation, float Damage);


	/**************************************************************************/
	/* State  */
	/**************************************************************************/

private:

	/** [Server + Client] Enable owning pawn control */
	void SetEnablePawnControl(bool Enable);

	/** [Client] OnRep method for bEnablePawnControl. Enable control for owning pawn */
	UFUNCTION()
	void OnRep_EnablePawnControl();
	
	/** [Server + Client] New pawn assigned. Reset PlayerTankMainWidget and enable pawn control  */
	virtual void AcknowledgePossession(APawn* P) override;

	/** Get this player GenericTeamID */
	FGenericTeamId GetGenericTeamId() const { return TeamId; }

};
