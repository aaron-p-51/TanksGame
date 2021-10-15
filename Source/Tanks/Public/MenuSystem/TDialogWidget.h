// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuWidget.h"
#include "TDialogWidget.generated.h"


class UTextBlock;

UENUM(BlueprintType)
enum class EDialogType : uint8
{
	EDT_SearchingForServers		UMETA(DisplayName = "SearchingForServers"),
	EDT_ConnectionError			UMETA(DisplayName = "ConnectionError"),
	EFT_Default					UMETA(DisplayName = "Default"),

	EFT_DefaultMax				UMETA(DisplayName = "Max")
};

/**
 * 
 */
UCLASS()
class TANKS_API UTDialogWidget : public UMenuWidget
{
	GENERATED_BODY()

/**
 * Members
 */
protected:


	EDialogType DialogType;

	FString DialogMessage;

 /**
  * Methods
  */

public:
	
	UTDialogWidget();

	//UTDialogWidget(FString DialogMessage, EDialogType DialogType);


	UFUNCTION(BlueprintCallable, Category = "MenuSystem")
	void SetDialogMessage(FString NewMessage, EDialogType Type);



	UFUNCTION(BlueprintImplementableEvent, Category = "MenuSystem")
	void SetupDialog(EDialogType Type);



	
	virtual void Setup() override;

	
};
