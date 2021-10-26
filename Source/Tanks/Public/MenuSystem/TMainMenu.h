// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuSystem/TMenuWidgetBase.h"
#include "TMainMenu.generated.h"

/** Forward declarations */
class UTSessionSubsystem;
class UEditableTextBox;
class UTServerRow;
class USlider;
class UButton;
class UTextBlock;
class UTErrorAckWidget;


/**
 * Class for Main Menu widget. Widget contains UI for player to navigate play options.
 */
UCLASS()
class TANKS_API UTMainMenu : public UTMenuWidgetBase
{
	GENERATED_BODY()
	
/**
 * Members
 */
protected:

	/** Server row class for each entry in ServerListScrollBox */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> ServerRowClass;

	/** Selected index in ServerListScrollBox */
	TOptional<uint32> SelectedServerRowIndex;


	/**************************************************************************/
	/* Widgets */
	/**************************************************************************/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MainMenuWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* MainMenuSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* MultiplayerSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* MultiplayerHostSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* MultiplayerJoinSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* SettingsSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* QuitGameConfirmWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* MenuButtonsSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* ConfirmQuitSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SinglePlayerButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SettingsBackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CancelConfirmQuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitConfirmQuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerHostButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerJoinButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerBackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* ServerNameTextBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* PlayerCountSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerCountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerHostBackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerHostHostButton;

	/** List to display found servers */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPanelWidget* ServerListScrollBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerJoinBackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerJoinRefreshButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MultiplayerJoinJoinButton;


/**
 * Methods
 */
protected:

	virtual bool Initialize() override;

	/** Bind widget events for widgets members */
	virtual bool BindWidgetEvents() override;

	/** Bind callbacks for SessionSubsystem events */
	virtual bool BindSessionSubsystemEvents() override;



	/**************************************************************************/
	/* Widget event bindings */
	/**************************************************************************/
private:
	UFUNCTION()
	void OnSinglePlayerButtonClick();

	UFUNCTION()
	void OnMultiplayerButtonClick();

	UFUNCTION()
	void OnSettingsButtonClick();

	UFUNCTION()
	void OnSettingsBackButtonClick();

	UFUNCTION()
	void OnExitButtonClick();

	UFUNCTION()
	void OnCancelConfirmQuitButtonClick();

	UFUNCTION()
	void OnQuitConfirmQuitButtonClick();

	UFUNCTION()
	void OnMultiplayerHostButtonClick();

	UFUNCTION()
	void OnMultiplayerJoinButtonClick();

	UFUNCTION()
	void OnMultiplayerBackButtonClick();

	UFUNCTION()
	void OnMultiplayerHostBackButtonClick();

	UFUNCTION()
	void OnMultiplayerHostHostButtonClick();

	UFUNCTION()
	void OnMultiplayerJoinBackButtonClick();

	UFUNCTION()
	void OnMultiplayerJoinRefreshButtonClick();

	UFUNCTION()
	void OnMultiplayerJoinJoinButtonClick();

	UFUNCTION()
	void OnPlayerCountSliderValueChange(float Value);


	/**************************************************************************/
	/* SessionSubsystem event callbacks */
	/**************************************************************************/
	UFUNCTION()
	void OnCreateSessionComplete(bool Successful);

	UFUNCTION()
	void OnStartSessionComplete(bool Successful);

	UFUNCTION()
	void OnDestroySessionComplete(bool Successful);

	void OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful);

	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);


	/**************************************************************************/
	/* Host Session Helpers */
	/**************************************************************************/
	/**
	 * Create a new session. This function should be modified as needed to set the session settings required
	 * for each session. For now we will only set the SessionName and PlayerCount. Must modify CreatSession function in TSessionSubsystem
	 * to allow and establish a new session based on additional session parameters
	 */
	UFUNCTION(BlueprintCallable)
	void CreateNewSession();

	/** After CreateNewSession is called and OnCreateSessionComplete is success. Call this function to start the session*/
	UFUNCTION(BlueprintCallable)
	void StartCreatedSession();

	/** After StartCreatedSession is called and OnStartSessionComplete is success. Call this function to travel to the next map */
	UFUNCTION(BlueprintCallable)
	void HostOpenLevelAfterSessionStart();

	void ConfigureMultiplayerHostMenu();


	/**************************************************************************/
	/* Join Session Helpers */
	/**************************************************************************/
	/** Update ServerListScrollBox when the user selects a session in ServerListScrollBox  */
	UFUNCTION(BlueprintCallable)
	void UpdateJoinServerList();

	/** Search for a new session to join. This function can be modified to allow specific specific search parameters */
	UFUNCTION(BlueprintCallable)
	void SearchForSessionsToJoin();

	/** After a session is selected in ServerListScrollBox attempt to join that session */
	UFUNCTION(BlueprintCallable)
	void JoinSelectedServer();

	/** Try and client travel to the session joined from JoinSelectedServer call. Call this function after OnJoinSessionComplete is successful  */
	void ClientTravelToJoinedSession();

	/** Using all of SessionResults found form the last FindSession call update the ServerListScrollBox with all of the found results */
	void FillServerListScrollBox(const TArray<FOnlineSessionSearchResult>& SessionResults);

public:

	/**
	 * Select a Server listed in ServerListScrollBox
	 * @param Index - Index of ServerListScrollBox child
	 */
	void SetSelectedServerRowIndex(uint32 Index);

};
