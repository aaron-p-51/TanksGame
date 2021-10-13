// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/TSessionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "TMenuWidgetBase.generated.h"


class UWidgetSwitcher;
class UTErrorAckWidget;

/**
 * 
 */
UCLASS()
class TANKS_API UTMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	

/**
 * Members
 */
protected:

	UPROPERTY(EditDefaultsOnly, Category = "ErrorAck")
	TSubclassOf<UTErrorAckWidget> ErrorAckWidgetClass;

	/** SessionSubsystem. Implements Session Interface to handle managing multilayer sessions  */
	UTSessionSubsystem* SessionSubsystem;

/**
 * Methods
 */

protected:

	virtual bool Initialize() override;

	virtual UTSessionSubsystem* GetSessionSubSystem();

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
	virtual bool SwitchSubmenu(UWidgetSwitcher* WidgetSwitcher, UWidget* SubmenuWidget);

	/**
	* Bind Widget Events, such as button click events for widgets in the menu. Called during Initialize.
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool BindWidgetEvents() { return true; }


	UFUNCTION(BlueprintCallable)
	virtual bool BindSessionSubsystemEvents() { return true; }

	/**
	 * Create and display UTErrorAckWidget. This widget child widgets will be be disabled when UTErrorAckWidget is visible on viewport.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void CreateErrorAckWidget(const FString& ErrorTitle, const FString& ErrorDetails);



};
