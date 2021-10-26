// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem\TMenuWidgetBase.h"

// Engine Includes
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetSwitcher.h"


// Game Includes
#include "MenuSystem/TErrorAckWidget.h"
#include "MenuSystem/TInfoDialogWidget.h"


bool UTMenuWidgetBase::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	SessionSubsystem = GetSessionSubSystem();

	bIsFocusable = true;
	Setup();
	if (!BindSessionSubsystemEvents()) return false;
	if (!BindWidgetEvents()) return false;

	CurrentDialogWidget = nullptr;

	return true;
}


UTSessionSubsystem* UTMenuWidgetBase::GetSessionSubSystem()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return nullptr;

	return GameInstance->GetSubsystem<UTSessionSubsystem>();
}


void UTMenuWidgetBase::Setup()
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


void UTMenuWidgetBase::TearDown()
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


bool UTMenuWidgetBase::SwitchSubmenu(UWidgetSwitcher* WidgetSwitcher, UWidget* SubmenuWidget)
{
	if (WidgetSwitcher && SubmenuWidget && WidgetSwitcher->HasChild(SubmenuWidget))
	{
		WidgetSwitcher->SetActiveWidget(SubmenuWidget);
		return true;
	}

	return false;
}


void UTMenuWidgetBase::CreateErrorAckWidget(const FString& ErrorTitle, const FString& ErrorDetails)
{
	if (!ErrorAckWidgetClass) return;

	// Check to see if UTErrorAckWidget already exists but is not visible
	TArray<UUserWidget*> FoundWidgets;
	UTErrorAckWidget* ErrorAckWidget = nullptr;

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, ErrorAckWidgetClass, false);
	for (auto& Widget : FoundWidgets)
	{
		ErrorAckWidget = Cast<UTErrorAckWidget>(Widget);
		if (ErrorAckWidget != nullptr)
		{
			break;
		}
	}

	if (!ErrorAckWidget)
	{
		ErrorAckWidget = CreateWidget<UTErrorAckWidget>(GetWorld(), ErrorAckWidgetClass);
	}

	if (ErrorAckWidget)
	{
		ErrorAckWidget->Configure(this, true, EInputModeOnClose::EIMOC_UIOnly, FText::FromString(ErrorTitle), FText::FromString(ErrorDetails));
	}	
}


void UTMenuWidgetBase::CreateInfoDialogWidget(const FString& DialogTitle, const FString& DialogDetails)
{
	if (!InfoDialogWidgetClass) return;

	TArray<UUserWidget*> FoundWidgets;
	UTInfoDialogWidget* InfoDailogWidget = nullptr;

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, InfoDialogWidgetClass, false);
	for (auto& Widget : FoundWidgets)
	{
		InfoDailogWidget = Cast<UTInfoDialogWidget>(Widget);
		if (InfoDailogWidget != nullptr)
		{
			break;
		}
	}

	if (!InfoDailogWidget)
	{
		InfoDailogWidget = CreateWidget<UTInfoDialogWidget>(GetWorld(), InfoDialogWidgetClass);
	}

	if (InfoDailogWidget)
	{
		InfoDailogWidget->Configure(this, true, EInputModeOnClose::EIMOC_UIOnly, FText::FromString(DialogTitle), FText::FromString(DialogDetails));
	}

	CurrentDialogWidget = InfoDailogWidget;
}


void UTMenuWidgetBase::RemoveCurrentInfoDialogWidget()
{
	if (!CurrentDialogWidget) return;

	CurrentDialogWidget->RemoveWidget();
	CurrentDialogWidget = nullptr;
}


