// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "TServerRow.generated.h"

/** Forward declarations */
class UTextBlock;
class UButton;
class UTMainMenu;

/**
 * Class for sessions to join. Display session info to player
 */
UCLASS()
class TANKS_API UTServerRow : public UUserWidget
{

	GENERATED_BODY()

/**
 * Members
 */
protected:
	
	UPROPERTY(BlueprintReadOnly)
	UTMainMenu* MainMenuWidget;

	/** Index of this Server, used when selecting Server from MainMenuWidget */
	uint32 Index;

	/** Session search results this server row represents */
	FOnlineSessionSearchResult ServerRowSearchResults;

	/* Specify if this ServerRow is selected in MainMenu*/
	UPROPERTY(BlueprintReadOnly)
	bool bIsSelected = false;

	/**************************************************************************/
	/* Widgets */
	/**************************************************************************/

	/** Name of Server */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerRowName;

	/** User who hosted the server */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerRowHost;

	/** Amount of players in server displayed as CurrentNumPlayers/MaxNumPlayers */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerRowPlayers;

	/** Each Server is a button which is selected within MainMain */
	UPROPERTY(meta = (BindWidget))
	UButton* ServerRowSelectButton;

	/**************************************************************************/
	/* Cosmetics */
	/**************************************************************************/

	/** Text color when this ServerRow is selected by player */
	UPROPERTY(EditDefaultsOnly)
	FSlateColor SelectedTextColor;

	/** Text color when this ServerRow is under player curser but not selected */
	UPROPERTY(EditDefaultsOnly)
	FSlateColor HoveredTextColor;

	/** Text color when this ServerRow is not selected or under player curser */
	UPROPERTY(EditDefaultsOnly)
	FSlateColor DefaultTextColor;

public:

	/** Header widget used as ServerRow column data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsHeader = false;

/**
 * Methods
 */

public:
	
	/** Set data for Server Row */
	void SetServerNameText(FText Text);
	void SetServerHostUserName(FText Text);
	void SetServerPlayersText(uint16 CurrentPlayers, uint16 MaxPlayers);
	void SetIsSelected(bool Selected);

	/**
	 * Setup ServerRow, will populate widget text field based on Results.
	 *
	 * @param InParent				Should always be the MainMenuWidget
	 * @param InIndex				Unique index for this ServerRow.
	 * @param Results				FOnlineSessionSearchResult represented by this ServerRow
	 */
	void Setup(UTMainMenu* InParent, uint32 InIndex, const FOnlineSessionSearchResult Result);

	FORCEINLINE FOnlineSessionSearchResult GetServerRowSearchResults() const { return ServerRowSearchResults; }

protected:

	
	virtual bool Initialize() override;

private:

	/** Set Widget properties */
	void SetServerRowData();

	/** Set the color all UUTextBlock widgets that are children of ServerRowSelectButton */
	void SetServerRowTextColor(FSlateColor Color);

	/**************************************************************************/
	/* Widget event bindings */
	/**************************************************************************/

	/** Sets this server Row as the selected index in Parent. Set color indicating it is selected */
	UFUNCTION()
	void ServerRowSelectButtonClicked();

	/** Set color indication players cursor is over this ServerRow */
	UFUNCTION()
	void ServerRowSelectButtonHovered();

	/** Set color indicating ServerRow is not selected or under players cursor  */
	UFUNCTION()
	void ServerRowSelectButtonUnhovered();
};
