// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/TMenuWidgetBase.h"
#include "TInGameMenu.generated.h"

/** Forward declarations */
class UButton;
class UTextBlock;
class UWidgetSwitcher;


/**
 * 
 */
UCLASS()
class TANKS_API UTInGameMenu : public UTMenuWidgetBase
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
	UWidgetSwitcher* InGameMenuSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* InGameMenuSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* InGameSettingsSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* ConfirmQuitWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* InGameMenuButtonsSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* InGameQuitSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CloseButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SettingsBackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ConfirmQuitCancelButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ConfirmQuitQuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ConfirmQuitText;



	
/**
 * Methods
 */
protected:

	/** Bind widget events for widgets members */
	virtual bool BindWidgetEvents() override;


	/**************************************************************************/
	/* Widget event bindings */
	/**************************************************************************/
private:

	UFUNCTION()
	void CloseButtonClick();

	UFUNCTION()
	void OnSettingsButtonClick();

	UFUNCTION()
	void OnSettingsBackButtonClick();

	UFUNCTION()
	void OnQuitButtonClick();

	UFUNCTION()
	void OnConfirmQuitCancelButtonClick();

	UFUNCTION()
	void OnConfirmQuitQuitButtonClick();
};
