// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/TMenuBase.h"

// Engine Includes
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

// Game Includes
#include "Subsystems/TSessionSubsystem.h"


bool UTMenuBase::Initialize()
{
	bool Success = Super::Initialize();
	if (Success)
	{
		bIsFocusable = true;
		Setup();

		// Must check for valid GameInstance, Initialize is called when
		// opening widgets in editor where no GameInstance exists.
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance)
		{
			SessionSubsystem = GetGameInstance()->GetSubsystem<UTSessionSubsystem>();
		}

		BindSessionSubsystemEvents();
		return BindWidgetEvents();
	}

	return false;
}



void UTMenuBase::Setup()
{
	this->AddToViewport();

	// Set the PlayerController input mode to interact with menu
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Set player control to UI input mode. Display mouse cursor.
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}


void UTMenuBase::TearDown()
{
	this->RemoveFromViewport();

	// Set PlayerController input mode to interact with game
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Set player control to Game input mode. Hide mouse cursor.
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);

		PlayerController->bShowMouseCursor = false;
	}
}


void UTMenuBase::SwitchSubmenu(UWidgetSwitcher* WidgetSwitcher, UWidget* SubmenuWidget)
{
	if (WidgetSwitcher && SubmenuWidget && WidgetSwitcher->HasChild(SubmenuWidget))
	{
		WidgetSwitcher->SetActiveWidget(SubmenuWidget);
	}
}
