// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPlayerTankMainWidget.generated.h"


class UWidgetSwitcher;
class UTextBlock;
class UProgressBar;
class UImage;
class ATPlayerTank;
class ATShootableWeapon;



/**
 * 
 */
UCLASS()
class TANKS_API UTPlayerTankMainWidget : public UUserWidget
{
	GENERATED_BODY()

/**
 * Members
 */
private:

	/**************************************************************************/
	/* Player */
	/**************************************************************************/

	UPROPERTY(VisibleAnywhere)
	ATPlayerTank* OwningPlayerTank;

	UPROPERTY()
	ATShootableWeapon* CurrentPlayerTankShootableWeapon;


	/**************************************************************************/
	/* Health */
	/**************************************************************************/

	UPROPERTY(EditDefaultsOnly)
	FLinearColor HealBarFillColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor LowHealthFillColor;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMax = "1.0", ClampMin = "0.0", UIMin = "0.0", UIMax = "1.0"))
	float MinHealthThreshold;

	float MaxHealthValue;


	/**************************************************************************/
	/* Boost */
	/**************************************************************************/

	float MaxBoostValue;


	/**************************************************************************/
	/* Timers */
	/**************************************************************************/

	FTimerHandle GameStartCoutDownTimer_TimerHandle;
	int32 GameStartCoutDownSeconds;

	FTimerHandle MatchInProgressCountDownTimer_TimeHandle;
	int32 MatchInProgressCountDownSeconds;
	
	
protected:

	/**************************************************************************/
	/* Widgets */
	/**************************************************************************/

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* PlayerTankMainWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* InGameOverlay;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* DeathOverlay;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TopStatusTextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* BottomStatusTextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MatchTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* BoostProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* WeaponReticleImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* WeaponIconImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* AmmoInMagazine;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* AmmoInCarry;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerKillerName;


 /**
  * Methods
  */
private:

	void SetupHeathCompBinding();
	void SetupBoostCompBinding();
	void SetupPlayerTankShootableWeaponBinding();

	void SetupHealthUI();
	void SetupBoostUI();
	void SetupShootableWeaponUI();



	UFUNCTION()
	void OnStartPlayerDelayStateChange(int32 StateTimeSeconds);
	UFUNCTION()
	void OnGameStartCountDownStateChange(int32 StateTimeSeconds);
	UFUNCTION()
	void OnGameInProgressStateChange(int32 StateTimeSeconds);
	UFUNCTION()
	void OnGameExitCountDownStateChange(int32 StateTimeSeconds);


	UFUNCTION()
	void StartMatchInProgressCountDown(int32 CountDownSeconds);

	UFUNCTION()
	void StartGameStartCountDown(int32 CountDownSeconds);

	UFUNCTION()
	void SetTopStatusMessage(const FString& Message);

	UFUNCTION()
	void SetBottomStatusMessage(const FText Message);

	UFUNCTION()
	void ClearStatusText(bool ClearTop = true, bool ClearBottom = true);

	ATPlayerTank* GetOwningPlayerTank() const;

	void ShowAllPlayerDataWidggets(bool Enable);

public:



	void SetupForNewPlayerTankPawn(ATPlayerTank* PlayerTank);

	virtual bool Initialize() override;

	//UFUNCTION()
	//void ResetMainWidget();


	UFUNCTION(BlueprintCallable)
	void SetNewHealthValue(float Value);

	UFUNCTION(BlueprintCallable)
	void SetNewBoostValue(float Value);

	UFUNCTION(BlueprintCallable)
	void SetWeaponIcons(ATShootableWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoDisplay(int32 CurrentAmmoInCarry, int32 CurrentAmmoInMagazine);



	UFUNCTION()
	void OnOwningPlayerTankHealthChange(/*class UTHealthComponent* HealthComp,*/ float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser);

	UFUNCTION()
	void OnOwningPlayerBoostChange(float Boost, bool UsingBoost);

	UFUNCTION()
	void OnOwningPlayerShootableWeaponChange(ATShootableWeapon* Weapon);

	UFUNCTION()
	void OnCurrentPlayerTankShootableWeaponAmmoChange(int32 CurrentAmmoInCarry, int32 CurrentAmmoInMagazine);

	void OwningPlayerTankKilled(const FString& KillerPlayerName);

protected:

	void UpdateMatchInProgressText();

	UFUNCTION()
	void MatchInProgressTick();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Delay();

	void RemovePlayerPawnBinding();

	/*void SetGameStateBinding();*/
	
	UFUNCTION()
	void GameStartCoutDownTick();

	void UpdateGameStartText();

	void ResetHealthBar();



	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetWeaponIconImage(ATShootableWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	void BindToGameStateStateChangeEvents();

};
