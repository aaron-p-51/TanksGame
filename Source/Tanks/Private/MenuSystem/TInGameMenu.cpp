// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuSystem/TInGameMenu.h"

// Engine Includes
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"






bool UTInGameMenu::BindWidgetEvents()
{
	if (!CloseButton) return false;
	if (!CloseButton->OnClicked.IsBound())
	{
		CloseButton->OnClicked.AddDynamic(this, &UTInGameMenu::CloseButtonClick);
	}

	if (!SettingsButton) return false;
	if (!SettingsButton->OnClicked.IsBound())
	{
		SettingsButton->OnClicked.AddDynamic(this, &UTInGameMenu::OnSettingsButtonClick);
	}

	if (!SettingsBackButton) return false;
	if (!SettingsBackButton->OnClicked.IsBound())
	{
		SettingsBackButton->OnClicked.AddDynamic(this, &UTInGameMenu::OnSettingsBackButtonClick);
	}

	if (!QuitButton) return false;
	if (!QuitButton->OnClicked.IsBound())
	{
		QuitButton->OnClicked.AddDynamic(this, &UTInGameMenu::OnQuitButtonClick);
	}

	if (!ConfirmQuitCancelButton) return false;
	if (!ConfirmQuitCancelButton->OnClicked.IsBound())
	{
		ConfirmQuitCancelButton->OnClicked.AddDynamic(this, &UTInGameMenu::OnConfirmQuitCancelButtonClick);
	}

	if (!ConfirmQuitQuitButton) return false;
	if (!ConfirmQuitQuitButton->OnClicked.IsBound())
	{
		ConfirmQuitQuitButton->OnClicked.AddDynamic(this, &UTInGameMenu::OnConfirmQuitQuitButtonClick);
	}

	UE_LOG(LogTemp, Warning, TEXT("BindWidgetEvents()"));
	return true;
}


/**************************************************************************/
/* Widget event bindings */
/**************************************************************************/
void UTInGameMenu::CloseButtonClick()
{
	TearDown();
	UE_LOG(LogTemp, Warning, TEXT("CloseButtonClick()"));
}


void UTInGameMenu::OnSettingsButtonClick()
{
	SwitchSubmenu(InGameMenuSwitcher, InGameSettingsSubmenu);
}


void UTInGameMenu::OnSettingsBackButtonClick()
{
	SwitchSubmenu(InGameMenuSwitcher, InGameMenuSubmenu);
}


void UTInGameMenu::OnQuitButtonClick()
{
	bool Success = SwitchSubmenu(ConfirmQuitWidgetSwitcher, InGameQuitSubmenu);
	if (Success && ConfirmQuitText)
	{
		APlayerController* OwningPlayerController =	GetOwningLocalPlayer()->GetPlayerController(GetWorld());
		if (OwningPlayerController && OwningPlayerController->HasAuthority())
		{
			ConfirmQuitText->SetText(FText::FromString(TEXT("Warning you are the Host. All clients will be disconnected!")));
		}
		else
		{
			ConfirmQuitText->SetText(FText::FromString(TEXT("Are you sure?")));
		}
	}
}


void UTInGameMenu::OnConfirmQuitCancelButtonClick()
{
	if (ConfirmQuitText)
	{
		ConfirmQuitText->SetText(FText::FromString(TEXT("")));
	}
	SwitchSubmenu(ConfirmQuitWidgetSwitcher, InGameMenuButtonsSubmenu);

}


void UTInGameMenu::OnConfirmQuitQuitButtonClick()
{
	UGameplayStatics::OpenLevel(GetWorld(), "/Game/Tanks/Map/MainMenu", true);
}
