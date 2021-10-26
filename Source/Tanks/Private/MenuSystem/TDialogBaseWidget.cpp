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
	// Return desired input mode to PlayerController
	switch (InputModeOnWidgetClose)
	{
		case EInputModeOnClose::EIMOC_GameOnly:
			SetGameInputMode();
			break;
		case EInputModeOnClose::EIMOC_UIOnly:
			SetUIInputMode();
			break;
		case EInputModeOnClose::EIMOC_GameAndUI:
			UE_LOG(LogTemp, Warning, TEXT("UTDialogBaseWidget::RemoveWidget - InputModeOnWidgetClose set to EIMOC_GameAndUI which is not implemented"))
			// TODO: Not implemented, may not be needed in this game
			break;
		default:
			break;
	}

	if (SpawningWidget)
	{
		SpawningWidget->SetIsEnabled(true);
	}

	this->RemoveFromViewport();
}

