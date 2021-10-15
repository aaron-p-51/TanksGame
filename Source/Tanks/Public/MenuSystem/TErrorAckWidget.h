// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TErrorAckWidget.generated.h"


class UButton;
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
class TANKS_API UTErrorAckWidget : public UUserWidget
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
	UTextBlock* TitleErrorText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ErrorDetailsText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CloseButton;

private:

	UPROPERTY()
	UUserWidget* SpawningWidget;

	UPROPERTY()
	EInputModeOnClose InputModeOnWidgetClose;
	

 /**
  * Methods
  */

public:

	UFUNCTION(BlueprintCallable)
	void SetTitleErrorText(const FText& Text);

	UFUNCTION(BlueprintCallable)
	void SetErrorDetailsText(const FText& Text);

	/**
	 * Configure Error acknowledgment window. Sets error text, and user input
	 * @param ErrorSpawningWidget			-	Widget that spawns this widget
	 * @param DisableInputToSpawningParent	-	Disable input to spawning widget, ie the widget this widget is displayed over
	 * @param ErrorTitle					-	Text to display as the error title or header
	 * @param ErrorDetails					-	Text to display description or details of the error
	 * @param InputModeOnClose				-	Input mode for local player controller when this widget is closed
	 */
	UFUNCTION(BlueprintCallable)
	void Configure(UUserWidget* ErrorSpawningWidget, bool DisableInputToSpawningParent, const FText& ErrorTitle, const FText& ErrorDetails, EInputModeOnClose InputModeOnClose);

private:

	/** Bind widget events */
	bool BindWidgetEvents();
	
	/** Bound event called when Click event is called on @CloseButton */
	UFUNCTION()
	void OnCloseButtonClick();

	/** Set the input mode for the local player controller to FInputModeUIOnly */
	void SetUIInputMode();

	/** Set the input mode for the local player controller to FInputModeGameOnly */
	void SetGameInputMode();
};
