// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TMenuInterface.h"
#include "MenuWidget.generated.h"

UENUM(BlueprintType)
enum class ETearDownOption : uint8
{
	ETDO_Remove		UMETA(DisplayName = "RemoveFromViewport"),
	ETDO_Destroy	UMETA(DisplayName = "Destroy"),

	ETDO_MAX		UMETA(DisplayName = "DefaultMax")
};


/**
 * 
 */
UCLASS()
class TANKS_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()



/*
* Methods *
*/

public:

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
	virtual void Setup();

	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UFUNCTION(BlueprintCallable, Category = "MenuWidget")
	void TearDown(ETearDownOption Option = ETearDownOption::ETDO_Remove);



protected:

	APlayerController* GetFirstPlayerControllerHelper();
	
};
