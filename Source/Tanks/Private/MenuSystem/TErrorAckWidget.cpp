// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem\TErrorAckWidget.h"

// Engine Includes
#include "Components/Button.h"
#include "Components/TextBlock.h"



void UTErrorAckWidget::Configure(UUserWidget* DialogSpawningWidget, bool DisableInputToSpawningParent, EInputModeOnClose InputModeOnClose, const FText& DialogTitleText, const FText& DialogDetailsText)
{
	Super::Configure(DialogSpawningWidget, DisableInputToSpawningParent, InputModeOnClose, DialogTitleText, DialogDetailsText);

	bIsFocusable = true;
	BindWidgetEvents();
	SetUIInputMode();
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

	RemoveWidget();
	/*if (InputModeOnWidgetClose == EInputModeOnClose::EIMOC_GameOnly)
	{
		SetGameInputMode();
	}

	if (SpawningWidget)
	{
		SpawningWidget->SetIsEnabled(true);
	}

	this->RemoveFromViewport();*/
}





