// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TPlayerTankHUD.h"

#include "UMG/TPlayerTankMainWidget.h"

void ATPlayerTankHUD::BeginPlay()
{
	Super::BeginPlay();

	//TankPlayerMainWidget = CreateWidget<UTPlayerTankMainWidget>(this, TankPlayerMainWidgetClass);
}

UTPlayerTankMainWidget* ATPlayerTankHUD::GetTankPlayerMainWidget() const
{
	return TankPlayerMainWidget;
}
