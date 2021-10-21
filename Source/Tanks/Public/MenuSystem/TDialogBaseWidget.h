// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDialogBaseWidget.generated.h"

class UTextBlock;


UENUM()
enum class EInputModeOnClose : uint8
{
	EIMOC_GameOnly	UMETA(DisplayName = "GameOnly"),
	EIMOC_UIOnly	UMETA(DisplayName = "UIOnly"),
	EIMOC_GameAndUI	UMETA(DisplayName = "GameAndUI")
};

/**
 * 
 */
UCLASS()
class TANKS_API UTDialogBaseWidget : public UUserWidget
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	/**************************************************************************/
	/* Widgets */
	/**************************************************************************/

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* DetailsText;

	UPROPERTY()
	UUserWidget* SpawningWidget;
	

	UPROPERTY()
	EInputModeOnClose InputModeOnWidgetClose;

/**
 * Methods
 */

 public:

	UFUNCTION(BlueprintCallable)
	void SetTitleText(const FText& Text);

	UFUNCTION(BlueprintCallable)
	void SetDetailsText(const FText& Text);

	/**
	 * Configure Error acknowledgment window. Sets error text, and user input
	 * @param DialogSpawningWidget			-	Widget that spawns this widget
	 * @param DisableInputToSpawningParent	-	Disable input to spawning widget, ie the widget this widget is displayed over
	 * @param InputModeOnClose				-	Input mode for local player controller when this widget is closed
	 * @param DialogTitleText				-	Text to display as the error title or header
	 * @param DialogDetailsText				-	Text to display description or details of the error
	 */
	UFUNCTION(BlueprintCallable)
	virtual void Configure(UUserWidget* DialogSpawningWidget, bool DisableInputToSpawningParent, EInputModeOnClose InputModeOnClose, const FText& DialogTitleText, const FText& DialogDetailsText);
	
	/** Set the input mode for the local player controller to FInputModeUIOnly */
	void SetUIInputMode();

	/** Set the input mode for the local player controller to FInputModeGameOnly */
	void SetGameInputMode();

	void RemoveWidget();
};
