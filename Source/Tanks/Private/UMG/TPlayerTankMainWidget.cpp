// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/TPlayerTankMainWidget.h"

// Engine Includes
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Game Includes
#include "Pawns/TPlayerTank.h"
#include "Weapons/TShootableWeapon.h"



static const float FULL_HEALTH = 1.f;

bool UTPlayerTankMainWidget::Initialize()
{
	return Super::Initialize();



}


void UTPlayerTankMainWidget::ResetMainWidget()
{
	if (PlayerTankMainWidgetSwitcher && InGameOverlay)
	{
		PlayerTankMainWidgetSwitcher->SetActiveWidget(InGameOverlay);
	}

	
	OwningPlayerTank = GetOwningPlayerTank();
	if (OwningPlayerTank)
	{	
		MaxHealthValue = OwningPlayerTank->GetMaxHealth();
		MaxBoostValue = 100.f;	// TODO: Create method to get max boost from owning player tank
	}


	ResetHealthBar();

	CurrentCountDownTimer = 0.f;

	BP_SetupComponentBinding();
	
}


void UTPlayerTankMainWidget::SetNewHealthValue(float Value)
{
	if (!HealthProgressBar) return;

	float HealthBarPercent = UKismetMathLibrary::MapRangeClamped(Value, 0.f, MaxHealthValue, 0.f, 1.f);
	HealthProgressBar->SetPercent(HealthBarPercent);
	if (HealthProgressBar->Percent > MinHealthThreshold)
	{
		HealthProgressBar->SetFillColorAndOpacity(LowHealthFillColor);
	}
	else
	{
		HealthProgressBar->SetFillColorAndOpacity(HealBarFillColor);
	}
}


void UTPlayerTankMainWidget::SetNewBoostValue(float Value)
{
	if (!BoostProgressBar) return;

	float BoostBarPercent = UKismetMathLibrary::MapRangeClamped(Value, 0.f, MaxBoostValue, 0.f, 1.f);
	BoostProgressBar->SetPercent(BoostBarPercent);
}


void UTPlayerTankMainWidget::SetWeaponIcons(ATShootableWeapon* Weapon)
{
	if (!Weapon) return;

	if (WeaponIconImage)
	{
		WeaponIconImage->SetBrushFromMaterial(Weapon->GetWeaponIcon());
	}

	if (WeaponReticleImage)
	{
		WeaponReticleImage->SetBrushFromTexture(Weapon->GetWeaponReticle());
	}
}


void UTPlayerTankMainWidget::UpdateAmmoDisplay(int32 CarryCount, int32 MagazineCount)
{
	if (AmmoInCarry)
	{
		AmmoInCarry->SetText(FText::AsNumber(CarryCount));
	}

	if (AmmoInMagazine)
	{
		AmmoInMagazine->SetText(FText::AsNumber(MagazineCount));
	}
}


void UTPlayerTankMainWidget::UpdateWeaponChange(ATShootableWeapon* Weapon)
{
	if (!Weapon) return;

	SetWeaponIcons(Weapon);
	UpdateAmmoDisplay(Weapon->GetCurrentAmmoInCarry(), Weapon->GetCurrentAmmoInMagazine());
}


void UTPlayerTankMainWidget::SetTopStatusMessage(const FText Message)
{
	if (!TopStatusTextBlock) return;

	TopStatusTextBlock->SetText(Message);
	TopStatusTextBlock->SetVisibility(ESlateVisibility::Visible);
	
	// FText TestMessage = FText::FromString("This is a test");
	// BP_ClearTopStatusMessage();
	// BP_SetTopStatusMessage(TestMessage);
}


void UTPlayerTankMainWidget::SetBottomStatusMessage(const FText Message)
{
	if (!BottomStatusTextBlock) return;

	BottomStatusTextBlock->SetText(Message);
	BottomStatusTextBlock->SetVisibility(ESlateVisibility::Visible);

	//BP_SetBottomStatusMessage(Message);
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


void UTPlayerTankMainWidget::StartMatchStartCountDown(const FText Message, int32 CountDownSeconds)
{
	BP_SetTopStatusMessage(Message);
	if (CountDownSeconds > 0)
	{
		BP_SetBottomStatusMessage(FText::FromString(FString::FromInt(CountDownSeconds)));
		CurrentCountDownTimer = CountDownSeconds;
		GetWorld()->GetTimerManager().SetTimer(CoutDownTimerHandle, this, &UTPlayerTankMainWidget::CountDownTimerTick, 1.f, true);
	}
	else
	{
		CountDownTimerTick();
	}
}


void UTPlayerTankMainWidget::ResetHealthBar()
{
	if (!HealthProgressBar) return;

	UE_LOG(LogTemp, Warning, TEXT("ResetHealthBar()"));

	HealthProgressBar->SetPercent(MaxHealthValue);
	HealthProgressBar->SetFillColorAndOpacity(HealBarFillColor);
}


ATPlayerTank* UTPlayerTankMainWidget::GetOwningPlayerTank() const
{
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) return nullptr;

	return Cast<ATPlayerTank>(PlayerPawn);
}


void UTPlayerTankMainWidget::CountDownTimerTick()
{
	CurrentCountDownTimer--;

	if (CurrentCountDownTimer <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CoutDownTimerHandle);
		BP_ClearTopStatusMessage();
		BP_ClearBottomStatusMessage();
	}
	else
	{
		BP_SetBottomStatusMessage(FText::FromString(FString::FromInt(CurrentCountDownTimer)));
	}
}
