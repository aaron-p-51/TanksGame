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


void UTErrorAckWidget::BindWidgetEvents()
{
	if (CloseButton && !CloseButton->OnClicked.IsAlreadyBound(this, &UTErrorAckWidget::OnCloseButtonClick))
	{
		CloseButton->OnClicked.AddDynamic(this, &UTErrorAckWidget::OnCloseButtonClick);
	}
}


void UTErrorAckWidget::OnCloseButtonClick()
{
	RemoveWidget();
}





