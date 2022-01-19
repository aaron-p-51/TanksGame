// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/TPlayerTankMainWidget.h"

// Engine Includes
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

// Game Includes
#include "Pawns/TPlayerTank.h"
#include "Weapons/TShootableWeapon.h"
#include "../Components/THealthComponent.h"
#include "Components/TBoostComponent.h"
#include "GameStates/TMultiplayerBaseGameState.h"



static const float FULL_HEALTH = 1.f;




bool UTPlayerTankMainWidget::Initialize()
{
	bool Success = Super::Initialize();

	if (WeaponIconImage)
	{
		//WeaponIconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TopStatusTextBlock)
	{
		TopStatusTextBlock->SetVisibility(ESlateVisibility::Hidden);
		TopStatusTextBlock->SetText(FText());
	}

	if (BottomStatusTextBlock)
	{
		BottomStatusTextBlock->SetVisibility(ESlateVisibility::Hidden);
		BottomStatusTextBlock->SetText(FText());
	}

	if (MatchTimeText)
	{
		MatchTimeText->SetVisibility(ESlateVisibility::Hidden);
		MatchTimeText->SetText(FText());
	}

	//BindToGameStateStateChangeEvents();

	return Success;
}


void UTPlayerTankMainWidget::BindToGameStateStateChangeEvents()
{
	UE_LOG(LogTemp, Warning, TEXT("UTPlayerTankMainWidget::BindToGameStateStateChangeEvents"));
	ATMultiplayerBaseGameState* GameState = GetWorld()->GetGameState<ATMultiplayerBaseGameState>();
	if (GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPlayerTankMainWidget::BindToGameStateStateChangeEvents: Has GameState"));
		GameState->OnStartPlayerDelay.AddDynamic(this, &UTPlayerTankMainWidget::OnStartPlayerDelayStateChange);
		GameState->OnGameStartCountDown.AddDynamic(this, &UTPlayerTankMainWidget::OnGameStartCountDownStateChange);
		GameState->OnGameInProgress.AddDynamic(this, &UTPlayerTankMainWidget::OnGameInProgressStateChange);
		GameState->OnGameExitCountDown.AddDynamic(this, &UTPlayerTankMainWidget::OnGameExitCountDownStateChange);
	}
}


void UTPlayerTankMainWidget::OnStartPlayerDelayStateChange(int32 StateTimeSeconds)
{
	SetTopStatusMessage(FString(TEXT("Waiting for players")));
	UE_LOG(LogTemp, Warning, TEXT("UTPlayerTankMainWidget::OnStartPlayerDelayStateChange"));
}


void UTPlayerTankMainWidget::OnGameStartCountDownStateChange(int32 StateTimeSeconds)
{
	SetTopStatusMessage(FString(TEXT("Match Starting In")));
	StartGameStartCountDown(StateTimeSeconds);
	UE_LOG(LogTemp, Warning, TEXT("UTPlayerTankMainWidget::OnGameStartCountDownStateChange"));
}


void UTPlayerTankMainWidget::OnGameInProgressStateChange(int32 StateTimeSeconds)
{
	ClearStatusText();

	StartMatchInProgressCountDown(StateTimeSeconds);

	// Is this called over and over again?

	UE_LOG(LogTemp, Warning, TEXT("UTPlayerTankMainWidget::OnGameInProgressStateChange"));
}


void UTPlayerTankMainWidget::OnGameExitCountDownStateChange(int32 StateTimeSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("UTPlayerTankMainWidget::OnGameExitCountDownStateChange"));
}


/******************************************************************************************/
/* Player Pawn Setup */
/******************************************************************************************/
void UTPlayerTankMainWidget::SetupForNewPlayerTankPawn(ATPlayerTank* PlayerTank)
{
	UE_LOG(LogTemp, Warning, TEXT("SetupForNewPlayerTankPawn"));
	if (PlayerTankMainWidgetSwitcher && InGameOverlay)
	{
		PlayerTankMainWidgetSwitcher->SetActiveWidget(InGameOverlay);
	}

	OwningPlayerTank = PlayerTank;

	if (OwningPlayerTank)
	{
		SetupBoostCompBinding();
		SetupBoostUI();

		SetupHeathCompBinding();
		SetupHealthUI();
			
		SetupPlayerTankShootableWeaponBinding();
		SetupShootableWeaponUI();	
	}
}


void UTPlayerTankMainWidget::SetupHeathCompBinding()
{
	UTHealthComponent* OwingPlayerTankHealthComp = OwningPlayerTank->GetHealthComponent();
	if (OwingPlayerTankHealthComp)
	{
		OwingPlayerTankHealthComp->OnHealthChange.AddDynamic(this, &UTPlayerTankMainWidget::OnOwningPlayerTankHealthChange);
	}
}


void UTPlayerTankMainWidget::SetupHealthUI()
{
	if (OwningPlayerTank)
	{
		MaxHealthValue = OwningPlayerTank->GetMaxHealth();
		SetNewHealthValue(OwningPlayerTank->GetHealth());
	}
}


void UTPlayerTankMainWidget::SetupBoostCompBinding()
{
	UTBoostComponent* OwningPlayerTankBoostComp = OwningPlayerTank->GetBoostComponent();
	if (OwningPlayerTankBoostComp)
	{
		OwningPlayerTankBoostComp->OnBoostChange.AddDynamic(this, &UTPlayerTankMainWidget::OnOwningPlayerBoostChange);
	}
}


void UTPlayerTankMainWidget::SetupBoostUI()
{
	if (OwningPlayerTank)
	{
		MaxBoostValue = OwningPlayerTank->GetMaxBoost();
		SetNewBoostValue(OwningPlayerTank->GetBoost());
	}
}


void UTPlayerTankMainWidget::SetupPlayerTankShootableWeaponBinding()
{
	OwningPlayerTank->OnShootableWeaponChange.AddDynamic(this, &UTPlayerTankMainWidget::OnOwningPlayerShootableWeaponChange);
}


void UTPlayerTankMainWidget::SetupShootableWeaponUI()
{
	if (OwningPlayerTank)
	{
		CurrentPlayerTankShootableWeapon = OwningPlayerTank->GetCurrentWeapon();
		if (CurrentPlayerTankShootableWeapon)
		{
			OnOwningPlayerShootableWeaponChange(CurrentPlayerTankShootableWeapon);
			SetWeaponIcons(CurrentPlayerTankShootableWeapon);
			UpdateAmmoDisplay(CurrentPlayerTankShootableWeapon->GetCurrentAmmoInCarry(), CurrentPlayerTankShootableWeapon->GetCurrentAmmoInMagazine());
		}
	}
}


/******************************************************************************************/
/* Health UI */
/******************************************************************************************/
void UTPlayerTankMainWidget::OnOwningPlayerTankHealthChange(/*UTHealthComponent* HealthComp,*/ float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser)
{
	SetNewHealthValue(Health);
}


void UTPlayerTankMainWidget::SetNewHealthValue(float Value)
{
	if (HealthProgressBar)
	{
		float HealthBarPercent = UKismetMathLibrary::MapRangeClamped(Value, 0.f, MaxHealthValue, 0.f, 1.f);
		HealthProgressBar->SetPercent(HealthBarPercent);
		if (HealthProgressBar->Percent > MinHealthThreshold)
		{
			HealthProgressBar->SetFillColorAndOpacity(HealBarFillColor);
		}
		else
		{
			HealthProgressBar->SetFillColorAndOpacity(LowHealthFillColor);
		}
	}
}


/******************************************************************************************/
/* Boost UI */
/******************************************************************************************/
void UTPlayerTankMainWidget::OnOwningPlayerBoostChange(float Boost, bool UsingBoost)
{
	SetNewBoostValue(Boost);
}


void UTPlayerTankMainWidget::SetNewBoostValue(float Value)
{
	if (BoostProgressBar)
	{
		float BoostBarPercent = UKismetMathLibrary::MapRangeClamped(Value, 0.f, MaxBoostValue, 0.f, 1.f);
		BoostProgressBar->SetPercent(BoostBarPercent);
	}
}


/******************************************************************************************/
/* Weapon UI */
/******************************************************************************************/
void UTPlayerTankMainWidget::OnOwningPlayerShootableWeaponChange(ATShootableWeapon* Weapon)
{
	if (Weapon)
	{
		if (CurrentPlayerTankShootableWeapon && CurrentPlayerTankShootableWeapon->OnAmmoAmountChange.IsAlreadyBound(this, &UTPlayerTankMainWidget::OnCurrentPlayerTankShootableWeaponAmmoChange))
		{
			CurrentPlayerTankShootableWeapon->OnAmmoAmountChange.RemoveDynamic(this, &UTPlayerTankMainWidget::OnCurrentPlayerTankShootableWeaponAmmoChange);
		}

		CurrentPlayerTankShootableWeapon = Weapon;
		CurrentPlayerTankShootableWeapon->OnAmmoAmountChange.AddDynamic(this, &UTPlayerTankMainWidget::OnCurrentPlayerTankShootableWeaponAmmoChange);
		UpdateAmmoDisplay(CurrentPlayerTankShootableWeapon->GetCurrentAmmoInCarry(), CurrentPlayerTankShootableWeapon->GetCurrentAmmoInMagazine());
		SetWeaponIcons(CurrentPlayerTankShootableWeapon);
	}
}


void UTPlayerTankMainWidget::SetWeaponIcons(ATShootableWeapon* Weapon)
{
	if (Weapon)
	{
		if (WeaponIconImage)
		{
			/*WeaponIconImage->SetVisibility(ESlateVisibility::Visible);
			WeaponIconImage->SetBrushFromMaterial(Weapon->GetWeaponIcon());*/
			BP_SetWeaponIconImage(Weapon);
		}

		if (WeaponReticleImage)
		{
			WeaponReticleImage->SetVisibility(ESlateVisibility::Visible);
			WeaponReticleImage->SetBrushFromTexture(Weapon->GetWeaponReticle());
		}
	}
}


void UTPlayerTankMainWidget::OnCurrentPlayerTankShootableWeaponAmmoChange(int32 CurrentAmmoInCarry, int32 CurrentAmmoInMagazine)
{
	UpdateAmmoDisplay(CurrentAmmoInCarry, CurrentAmmoInMagazine);
}




void UTPlayerTankMainWidget::UpdateAmmoDisplay(int32 CurrentAmmoInCarry, int32 CurrentAmmoInMagazine)
{
	if (AmmoInCarry)
	{
		AmmoInCarry->SetText(FText::AsNumber(CurrentAmmoInCarry));
	}

	if (AmmoInMagazine)
	{
		AmmoInMagazine->SetText(FText::AsNumber(CurrentAmmoInMagazine));
	}
}



void UTPlayerTankMainWidget::OwningPlayerTankKilled(const FString& KillerPlayerName)
{
	// Hide Widgets
	if (PlayerTankMainWidgetSwitcher && DeathOverlay)
	{
		PlayerTankMainWidgetSwitcher->SetActiveWidget(DeathOverlay);
	}
	if (PlayerKillerName)
	{
		PlayerKillerName->SetText(FText::FromString(KillerPlayerName));
	}
}


void UTPlayerTankMainWidget::ResetHealthBar()
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(1.f);
		HealthProgressBar->SetFillColorAndOpacity(HealBarFillColor);
	}
}


ATPlayerTank* UTPlayerTankMainWidget::GetOwningPlayerTank() const
{
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) return nullptr;

	return Cast<ATPlayerTank>(PlayerPawn);
}




void UTPlayerTankMainWidget::StartGameStartCountDown(int32 CountDownSeconds)
{
	if (CountDownSeconds > 0)
	{
		GameStartCoutDownSeconds = CountDownSeconds;
		UpdateGameStartText();
		GetWorld()->GetTimerManager().ClearTimer(GameStartCoutDownTimer_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(GameStartCoutDownTimer_TimerHandle, this, &UTPlayerTankMainWidget::GameStartCoutDownTick, 1.f, true);
	}
	else
	{
		GameStartCoutDownTick();
	}
}


void UTPlayerTankMainWidget::GameStartCoutDownTick()
{
	GameStartCoutDownSeconds--;

	if (GameStartCoutDownSeconds <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameStartCoutDownTimer_TimerHandle);
	}
	else
	{
		UpdateGameStartText();
	}
}


void UTPlayerTankMainWidget::UpdateGameStartText()
{
	SetBottomStatusMessage(FText::FromString(FString::FromInt(GameStartCoutDownSeconds)));
}


void UTPlayerTankMainWidget::StartMatchInProgressCountDown(int32 CountDownSeconds)
{
	if (CountDownSeconds > 0)
	{
		MatchInProgressCountDownSeconds = CountDownSeconds;
		UpdateMatchInProgressText();
		GetWorld()->GetTimerManager().ClearTimer(MatchInProgressCountDownTimer_TimeHandle);
		GetWorld()->GetTimerManager().SetTimer(MatchInProgressCountDownTimer_TimeHandle, this, &UTPlayerTankMainWidget::MatchInProgressTick, 1.f, true);
	}
	else
	{
		MatchInProgressTick();
	}
}


void UTPlayerTankMainWidget::MatchInProgressTick()
{
	--MatchInProgressCountDownSeconds;

	if (MatchInProgressCountDownSeconds < 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(MatchInProgressCountDownTimer_TimeHandle);
		MatchTimeText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UpdateMatchInProgressText();
	}
}


void UTPlayerTankMainWidget::UpdateMatchInProgressText()
{
	if (!MatchTimeText) return;

	int32 Minutes = MatchInProgressCountDownSeconds / 60;
	int32 Seconds = MatchInProgressCountDownSeconds % 60;

	// Format string to have leading 0 in seconds place if there are less than 10 seconds in the current minute
	FString TimeString;
	if (Seconds > 9)
	{
		TimeString = FString::Printf(TEXT("%d:%d"), Minutes, Seconds);
	}
	else
	{
		TimeString = FString::Printf(TEXT("%d:0%d"), Minutes, Seconds);
	}

	MatchTimeText->SetText(FText::FromString(TimeString));
	MatchTimeText->SetVisibility(ESlateVisibility::Visible);

}

/************************************************************/
/* Update UI */
/************************************************************/
void UTPlayerTankMainWidget::SetTopStatusMessage(const FString& Message)
{
	if (!TopStatusTextBlock) return;

	TopStatusTextBlock->SetText(FText::FromString(Message));
	TopStatusTextBlock->SetVisibility(ESlateVisibility::Visible);
}


void UTPlayerTankMainWidget::SetBottomStatusMessage(const FText Message)
{
	if (!BottomStatusTextBlock) return;

	BottomStatusTextBlock->SetText(Message);
	BottomStatusTextBlock->SetVisibility(ESlateVisibility::Visible);
}


void UTPlayerTankMainWidget::ClearStatusText(bool ClearTop, bool ClearBottom)
{
	if (ClearTop && TopStatusTextBlock)
	{
		TopStatusTextBlock->SetText(FText());
		TopStatusTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}

	if (ClearBottom && BottomStatusTextBlock)
	{
		BottomStatusTextBlock->SetText(FText());
		BottomStatusTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
}
