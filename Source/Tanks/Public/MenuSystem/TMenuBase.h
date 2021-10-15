// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TMenuBase.generated.h"

class UTSessionSubsystem;
class UWidgetSwitcher;
class UWidget;

/**
 * 
 */
UCLASS()
class TANKS_API UTMenuBase : public UUserWidget
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	/** SessionSubsystem. Implements Session Interface to handle managing multilayer sessions  */
	UTSessionSubsystem* SessionSubsystem;

/**
 * Methods
 */

protected:

	/** Initialize menu Widget, will set focus to this widget and call BindWidgetEvents, and BindSessionSubsystemEvents functions */
	virtual bool Initialize() override;

	/**
	 * Switch UWidgetSwitcher child class.
	 *
	 * @param WidgetSwitcher		WidgetSwitcher Widget to switch current child. Must be parent of SubmenuWidget
	 * @param SubmenuWidget			Child of WidgetSwitcher to switch to. Must be child of WidgetSwitcher
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SwitchSubmenu(UWidgetSwitcher* WidgetSwitcher, UWidget* SubmenuWidget);

	/**
	 * Bind Widget Events, such as button click events for widgets in the menu. Called during Initialize.
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool BindWidgetEvents() PURE_VIRTUAL(UMMenuBase::BindWidgetEvents, return false;);

	/**
	 * Bind SessionSubsystem events. Called during Initialize. You do not need to bind all of the events fired from
	 * SessionSubsystem, just the ones that are called from this Menu Widget.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void BindSessionSubsystemEvents() PURE_VIRTUAL(UMMenuBase::BindWidgetEvents, return;);

	/** Setup Widget. Adds widget to viewport, sets input mode to UI and show mouse courser for player */
	UFUNCTION(BlueprintCallable)
	virtual void Setup();

	/** Remove widget from viewport, sets input mode for game only and hides mouse courser */
	UFUNCTION(BlueprintCallable)
	virtual void TearDown();
	
};
