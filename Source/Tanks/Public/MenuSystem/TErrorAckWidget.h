// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDialogBaseWidget.h"
#include "TErrorAckWidget.generated.h"

/** Forward declarations */
class UButton;


/**
 * Widget that displays errors to the player. Widgets are closed manually by the player
 */
UCLASS()
class TANKS_API UTErrorAckWidget : public UTDialogBaseWidget
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
	UButton* CloseButton;

 /**
  * Methods
  */

public:

	/**
	 * Configure Error acknowledgment window. Sets error text, and user input
	 * @param DialogSpawningWidget			-	Widget that spawns this widget
	 * @param DisableInputToSpawningParent	-	Disable input to spawning widget, ie the widget this widget is displayed over
	 * @param InputModeOnClose				-	Input mode for local player controller when this widget is closed
	 * @param DialogTitleText				-	Text to display as the error title or header
	 * @param DialogDetailsText				-	Text to display description or details of the error
	 */
	virtual void Configure(UUserWidget* DialogSpawningWidget, bool DisableInputToSpawningParent, EInputModeOnClose InputModeOnClose, const FText& DialogTitleText, const FText& DialogDetailsText) override;

private:

	/** Bind widget events */
	void BindWidgetEvents();
	
	/** Bound event called when Click event is called on @CloseButton */
	UFUNCTION()
	void OnCloseButtonClick();
};
