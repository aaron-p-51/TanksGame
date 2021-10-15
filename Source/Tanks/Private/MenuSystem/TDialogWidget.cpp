// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\MenuSystem\TDialogWidget.h"

#include "Components/TextBlock.h"


UTDialogWidget::UTDialogWidget()
{
	DialogType = EDialogType::EFT_Default;
}


void UTDialogWidget::SetDialogMessage(FString NewMessage, EDialogType Type)
{
	DialogMessage = NewMessage;
	DialogType = Type;
}


void UTDialogWidget::Setup()
{
	Super::Setup();

	if (DialogType != EDialogType::EFT_Default)
	{
		SetupDialog(DialogType);
	}
}
