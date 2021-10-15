// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\MenuSystem\MenuWidget.h"


void UMenuWidget::Setup()
{
	this->AddToViewport();
	APlayerController* PC = GetFirstPlayerControllerHelper();
	if (PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		//InputMode.SetWidgetToFocus(this->TakeWidget());
		PC->bShowMouseCursor = true;
		PC->SetInputMode(InputMode);
	}
}


void UMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	this->RemoveFromViewport();

	APlayerController* PC = GetFirstPlayerControllerHelper();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameOnly());
	}
}


void UMenuWidget::TearDown(ETearDownOption Option)
{

	APlayerController* PC = GetFirstPlayerControllerHelper();
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	switch (Option)
	{
	case ETearDownOption::ETDO_Remove:
		this->RemoveFromViewport();
		break;
	case ETearDownOption::ETDO_Destroy:
		Destruct();
		break;
	case ETearDownOption::ETDO_MAX:
	default:
		break;
	}

	return;
}



APlayerController* UMenuWidget::GetFirstPlayerControllerHelper()
{
	UWorld* World = GetWorld();
	if (World)
	{
		return World->GetFirstPlayerController();
	}

	return nullptr;
}

