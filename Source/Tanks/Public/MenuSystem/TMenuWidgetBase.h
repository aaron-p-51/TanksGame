// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/TSessionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "TMenuWidgetBase.generated.h"

/** Forward declarations */
class UWidgetSwitcher;
class UTErrorAckWidget;
class UTInfoDialogWidget;

/**
 * Base class for menu widgets
 */
UCLASS()
class TANKS_API UTMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	

/**
 * Members
 */
protected:

	/** Class used to display errors to the player */
	UPROPERTY(EditDefaultsOnly, Category = "ErrorAck")
	TSubclassOf<UTErrorAckWidget> ErrorAckWidgetClass;

	/** Class used display info to the player */
	UPROPERTY(EditDefaultsOnly, Category = "DialogWidget")
	TSubclassOf<UTInfoDialogWidget> InfoDialogWidgetClass;

	/** This class must remove all widgets of type InfoDialogWidgetClass it creates. Store the currently displayed UTInfoDialogWidget */
	UPROPERTY()
	UTInfoDialogWidget* CurrentDialogWidget;

	/** SessionSubsystem. Implements Session Interface to handle managing multilayer sessions  */
	UTSessionSubsystem* SessionSubsystem;

/**
 * Methods
 */

protected:

	/** Setup the menu widget, get SessionSubsystem, set initial state of widget, This function should be overridden again in
	 * derived classes that will need to bind widget events and/or will use the Session Subsystem*/
	virtual bool Initialize() override;

	/** Get UTSessionSubsystem  */
	UTSessionSubsystem* GetSessionSubSystem();

	/** Setup Widget. Adds widget to viewport, sets input mode to UI and show mouse courser for player */
	UFUNCTION(BlueprintCallable)
	virtual void Setup();

	/** Remove widget from viewport, sets input mode for game only and hides mouse courser */
	UFUNCTION(BlueprintCallable)
	virtual void TearDown();

	/**
	 * Switch UWidgetSwitcher child class.
	 *
	 * @param WidgetSwitcher		WidgetSwitcher Widget to switch current child. Must be parent of SubmenuWidget
	 * @param SubmenuWidget			Child of WidgetSwitcher to switch to. Must be child of WidgetSwitcher
	 * 
	 * @return	Submenu switched
	 */
	UFUNCTION(BlueprintCallable)
	bool SwitchSubmenu(UWidgetSwitcher* WidgetSwitcher, UWidget* SubmenuWidget);

	/**
	 * Bind Widget Events, such as button click events for widgets in the menu. Called during Initialize. Is not required to override if no
	 * widget event binding is required
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool BindWidgetEvents() { return true; }

	/** 
	 * Bind events to Online Subsystem callbacks. See events in @TSessionSubsystem.h. Is not required to oveerride if widget does not use
	 * Online Subsystem
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool BindSessionSubsystemEvents() { return true; }

	/** Create and display UTErrorAckWidget. This widget will be disabled when UTErrorAckWidget is visible on viewport. */
	UFUNCTION(BlueprintCallable)
	void CreateErrorAckWidget(const FString& ErrorTitle, const FString& ErrorDetails);

	/** 
	 * Create and display TInfoDialogWidget. This widget will be disabled when TInfoDialogWidget is visible on viewport. Player can not
	 * close a TInfoDialogWidget. Must be closed by calling RemoveCurrentInfoDialogWidget() */
	UFUNCTION(BlueprintCallable)
	void CreateInfoDialogWidget(const FString& DialogTitle, const FString& DialogDetails);

	/** Remove the current TInfoDialogWidget from the viewport  */
	UFUNCTION(BlueprintCallable)
	void RemoveCurrentInfoDialogWidget();

};
