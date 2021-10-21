// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/TDialogBaseWidget.h"

// Engine Includes
#include "Components/TextBlock.h"


void UTDialogBaseWidget::Configure(UUserWidget* DialogSpawningWidget, bool DisableInputToSpawningParent, EInputModeOnClose InputModeOnClose, const FText& DialogTitleText, const FText& DialogDetailsText)
{
	SetTitleText(DialogTitleText);
	SetDetailsText(DialogDetailsText);
	InputModeOnWidgetClose = InputModeOnClose;

	SpawningWidget = DialogSpawningWidget;
	if (SpawningWidget && DisableInputToSpawningParent)
	{
		SpawningWidget->SetIsEnabled(false);
	}

	this->AddToViewport();
}


void UTDialogBaseWidget::SetTitleText(const FText& Text)
{
	if (TitleText)
	{
		TitleText->SetText(Text);
	}
}


void UTDialogBaseWidget::SetDetailsText(const FText& Text)
{
	if (DetailsText)
	{
		DetailsText->SetText(Text);
	}
}

void UTDialogBaseWidget::SetUIInputMode()
{
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


void UTDialogBaseWidget::SetGameInputMode()
{
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

void UTDialogBaseWidget::RemoveWidget()
{
	if (InputModeOnWidgetClose == EInputModeOnClose::EIMOC_GameOnly)
	{
		SetGameInputMode();
	}

	if (SpawningWidget)
	{
		SpawningWidget->SetIsEnabled(true);
	}

	this->RemoveFromViewport();
}

