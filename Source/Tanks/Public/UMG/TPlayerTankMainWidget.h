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

	/**************************************************************************/
	/* Health */
	/**************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor HealBarFillColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor LowHealthFillColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMax = "1.0", ClampMin = "0.0", UIMin = "0.0", UIMax = "1.0"))
	float MinHealthThreshold;

	float MaxHealthValue;

	/**************************************************************************/
	/* Boost */
	/**************************************************************************/

	float MaxBoostValue;







	UPROPERTY(BlueprintReadWrite)
	ATPlayerTank* OwningPlayerTank;

	

	int32 CurrentCountDownTimer;

	//int32 CountDownTimerDuration;

	FTimerHandle CoutDownTimerHandle;


 /**
  * Methods
  */


protected:

	

public:


 virtual bool Initialize() override;

  UFUNCTION()
  void ResetMainWidget();

  UFUNCTION(BlueprintCallable)
  void SetNewHealthValue(float Value);

  UFUNCTION(BlueprintCallable)
  void SetNewBoostValue(float Value);

  UFUNCTION(BlueprintCallable)
  void SetWeaponIcons(ATShootableWeapon* Weapon);

  UFUNCTION(BlueprintCallable)
  void UpdateAmmoDisplay(int32 CarryCount, int32 MagazineCount);

  UFUNCTION(BlueprintCallable)
  void UpdateWeaponChange(ATShootableWeapon* Weapon);



  UFUNCTION()
  void SetTopStatusMessage(const FText Message);

  UFUNCTION()
  void SetBottomStatusMessage(const FText Message);

  UFUNCTION()
  void ClearStatusText(bool ClearTop, bool ClearBottom);

  UFUNCTION()
  void StartMatchStartCountDown(const FText Message, int32 CountDownSeconds);

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ResetHealthBar();

	
	void ResetHealthBar();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetupComponentBinding();

  UFUNCTION(BlueprintImplementableEvent)
  void BP_SetNewHealthFillPercent(float Value);

  UFUNCTION(BlueprintImplementableEvent)
  void BP_SetTopStatusMessage(const FText& Message);

  UFUNCTION(BlueprintImplementableEvent)
  void BP_SetBottomStatusMessage(const FText& Message);

  UFUNCTION(BlueprintImplementableEvent)
  void BP_ClearBottomStatusMessage();

  UFUNCTION(BlueprintImplementableEvent)
  void BP_ClearTopStatusMessage();

  UFUNCTION(BlueprintImplementableEvent)
  void BP_StartCountDownTimer();


  ATPlayerTank* GetOwningPlayerTank() const;

  void CountDownTimerTick();



 

	
};
