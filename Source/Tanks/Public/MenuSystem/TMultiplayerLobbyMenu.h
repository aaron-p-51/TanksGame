// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/TMenuWidgetBase.h"
#include "TMultiplayerLobbyMenu.generated.h"

/** Forward declarations */
class ATLobbyGameState;
class UTextBlock;
class UCheckBox;
class UWidgetSwitcher;
class UButton;
class UVerticalBox;
class UTPlayerInLobby;
class USizeBox;
class UTSessionSubsystem;

/**
 * Menu widget for multilayer lobby map
 */
UCLASS()
class TANKS_API UTMultiplayerLobbyMenu : public UTMenuWidgetBase
{
	GENERATED_BODY()

/**
 * Members  
 */
protected:

	UPROPERTY()
	ATLobbyGameState* LobbyGameState;

	/** Widget to show player name */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTPlayerInLobby> PlayerInLobbyWidgetClass;

	/** Maps current players with instance of PlayerInLobbyWidgetClass */
	UPROPERTY()
	TMap<APlayerState*, UTPlayerInLobby*> PlayerInLobbyMap;

	/** Data update rate UI to show current session data */
	UPROPERTY(EditDefaultsOnly, Category = "Data Update")
	float UpdateLobbyDataRate = 0.3f;

	/** Timer handle to update UI */
	FTimerHandle TimerHandler_UpdateLobbyData;

	
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
	UTextBlock* MatchStartText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USizeBox* NextMapBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USizeBox* VoteToStartBox;

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

	/** Set timer to update UI data, init all widget properties before updates start  */
	virtual bool Initialize() override;

	/** Bind widget events for widgets members above */
	virtual bool BindWidgetEvents() override;

	/** Bind callbacks for SessionSubsystem events */
	virtual bool BindSessionSubsystemEvents() override;

	/** Get the game state used for Multilayer Player Lobby */
	ATLobbyGameState* GetLobbyGameState() const;

	/**************************************************************************/
	/* Widget event bindings */
	/**************************************************************************/
private:

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

	/**************************************************************************/
	/* Update UI */
	/**************************************************************************/

	void UpdateLobbyData();

	/** Set widget properties before first update. Makes the UI look cleaner before first update */
	void InitWidgetProperties();

	/** Updates the UI every tick of TimerHandler_UpdateLobbyData */
	void UpdateServerName(const FString& ServerName);
	void UpdateVotesToStartText(int32 Votes, int32 VotesNeeded);
	void UpdatePlayersInLobbyCount(const int32 CurrentPlayers, const int32 MaxPlayers);
	void UpdatePlayersInLobby(ATLobbyGameState* GameState);
	void UpdateTimeTillMatchStart(int32 SecondsTillMatchStart);
	
	
};
