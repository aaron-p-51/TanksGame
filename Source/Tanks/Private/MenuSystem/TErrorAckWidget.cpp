// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem\TErrorAckWidget.h"

// Engine Includes
#include "Components/Button.h"
#include "Components/TextBlock.h"



void UTErrorAckWidget::Configure(UUserWidget* ErrorSpawningWidget, bool DisableInputToSpawningParent, const FText& ErrorTitle, const FText& ErrorDetails, EInputModeOnClose InputModeOnClose)
{
	bIsFocusable = true;
	BindWidgetEvents();
	SetUIInputMode();
	SetTitleErrorText(ErrorTitle);
	SetErrorDetailsText(ErrorDetails);
	InputModeOnWidgetClose = InputModeOnClose;

	SpawningWidget = ErrorSpawningWidget;
	if (SpawningWidget && DisableInputToSpawningParent)
	{
		SpawningWidget->SetIsEnabled(false);
	}

	this->AddToViewport();
}


bool UTErrorAckWidget::BindWidgetEvents()
{
	if (!CloseButton) return false;
	if (!CloseButton->OnClicked.IsAlreadyBound(this, &UTErrorAckWidget::OnCloseButtonClick))
	{
		CloseButton->OnClicked.AddDynamic(this, &UTErrorAckWidget::OnCloseButtonClick);
	}

	return true;
}


void UTErrorAckWidget::OnCloseButtonClick()
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


void UTErrorAckWidget::SetUIInputMode()
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


void UTErrorAckWidget::SetGameInputMode()
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


void UTErrorAckWidget::SetTitleErrorText(const FText& Text)
{
	if (TitleErrorText)
	{
		TitleErrorText->SetText(Text);
	}
}


void UTErrorAckWidget::SetErrorDetailsText(const FText& Text)
{
	if (ErrorDetailsText)
	{
		ErrorDetailsText->SetText(Text);
	}
}


