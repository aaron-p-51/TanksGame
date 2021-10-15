// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/TMenuWidgetBase.h"
#include "TMultiplayerLobbyMenu.generated.h"

class ATLobbyGameState;
class UTextBlock;
class UCheckBox;
class UWidgetSwitcher;
class UButton;
class UVerticalBox;
class UTPlayerInLobby;
class UTSessionSubsystem;

/**
 * 
 */
UCLASS()
class TANKS_API UTMultiplayerLobbyMenu : public UTMenuWidgetBase
{
	GENERATED_BODY()

/**
 * Members  
 */
protected:

	///** SessionSubsystem. Implements Session Interface to handle managing multilayer sessions  */
	//UTSessionSubsystem* SessionSubsystem;

	UPROPERTY()
	ATLobbyGameState* LobbyGameState;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTPlayerInLobby> PlayerInLobbyWidgetClass;
	

	/**************************************************************************/
	/* Widgets */
	/**************************************************************************/

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ServerNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerCountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MaxPlayersText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* MatchStartTimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* VoteToStartCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CurrentVotesToStartText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NeededVotesToStartText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* QuitWidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* QuitSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidget* ConfirmQuitSubmenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* QuitToMainMenuButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ConfirmQuitText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ConfirmQuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CancelQuitButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* PlayersBox;



	



/**
 * Methods
 */

protected:

	//virtual bool Initialize() override;

	/** Bind widget events for widgets members above */
	virtual bool BindWidgetEvents() override;

	/** Bind callbacks for SessionSubsystem events */
	virtual bool BindSessionSubsystemEvents() override;

	/**************************************************************************/
	/* Widget event bindings */
	/**************************************************************************/

	UFUNCTION()
	void OnVoteToStartCheckBoxChange(bool IsChecked);

	UFUNCTION()
	void OnQuitToMainMenuButtonClick();

	UFUNCTION()
	void OnConfirmQuitButtonClick();

	UFUNCTION()
	void OnCancelQuitButtonClick();

	/**************************************************************************/
	/* SessionSubsystem event callbacks */
	/**************************************************************************/

	UFUNCTION()
	void OnEndSessionComplete(bool Successful);


 private:

	 ATLobbyGameState* GetLobbyGameState() const;

	/** Update the current vote count text widgets needed to start match */
	void UpdateVotesToStartText(int32 Votes, int32 VotesNeeded);

	/** Update the Server/Session name */
	void UpdateServerName(const FString& ServerName);

	void UpdateTimeTillMatchStart(int32 SecondsTillMatchStart);

	void UpdatePlayersInLobby(ATLobbyGameState* GameState);

	void UpdatePlayersInLobbyCount(const int32 CurrentPlayers, const int32 MaxPlayers);


 public:
	
	UFUNCTION(BlueprintCallable)
	void UpdateLobbyDataNew();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	FString GetTotalPlayers();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerList();


	
};
