// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MenuSystem/TServerRow.h"

// Engine Includes
#include "Components/TextBlock.h"
#include "Components/Button.h"

// Game Includes
#include "MenuSystem/TMainMenu.h"

bool UTServerRow::Initialize()
{
	bool Success = Super::Initialize();
	if (Success && bIsHeader)
	{
		this->SetIsEnabled(false);
	}

	return Success;
}


void UTServerRow::Setup(UTMainMenu* InParent, uint32 InIndex, const FOnlineSessionSearchResult Result)
{
	MainMenuWidget = InParent;
	Index = InIndex;
	ServerRowSearchResults = Result;

	if (ServerRowSelectButton)
	{	
		if (!ServerRowSelectButton->OnClicked.IsBound())
		{
			ServerRowSelectButton->OnClicked.AddDynamic(this, &UTServerRow::ServerRowSelectButtonClicked);
		}
		if (!ServerRowSelectButton->OnHovered.IsBound())
		{
			ServerRowSelectButton->OnHovered.AddDynamic(this, &UTServerRow::ServerRowSelectButtonHovered);
		}
		if (!ServerRowSelectButton->OnUnhovered.IsBound())
		{
			ServerRowSelectButton->OnUnhovered.AddDynamic(this, &UTServerRow::ServerRowSelectButtonUnhovered);
		}
	}
	
	SetServerRowData();
}


void UTServerRow::SetServerRowData()
{
	// Set the server name in the new server row entry. This is what the host named the server
	FString ServerName;
	if (ServerRowSearchResults.Session.SessionSettings.Get(SETTING_MAPNAME, ServerName))
	{
		SetServerNameText(FText::FromString(ServerName));
	}

	// Set the server host name (Player name) in the new server row entry
	SetServerHostUserName(FText::FromString(ServerRowSearchResults.Session.OwningUserName));

	// Set the current and max players in the new server row entry
	int32 MaxPlayers = ServerRowSearchResults.Session.SessionSettings.NumPublicConnections;
	int32 CurrentPlayers = MaxPlayers - ServerRowSearchResults.Session.NumOpenPublicConnections;
	SetServerPlayersText(CurrentPlayers, MaxPlayers);
}


void UTServerRow::SetServerNameText(FText Text)
{
	if (ServerRowName)
	{
		ServerRowName->SetText(Text);
	}
}


void UTServerRow::SetServerHostUserName(FText Text)
{
	if (ServerRowHost)
	{
		ServerRowHost->SetText(Text);
	}
}


void UTServerRow::SetServerPlayersText(uint16 CurrentPlayers, uint16 MaxPlayers)
{
	if (ServerRowPlayers)
	{
		FString Text = FString::Printf(TEXT("%d/%d"), CurrentPlayers, MaxPlayers);
		ServerRowPlayers->SetText(FText::FromString(Text));
	}
}


void UTServerRow::SetIsSelected(bool Selected)
{
	bIsSelected = Selected;

	if (!bIsSelected)
	{
		SetServerRowTextColor(DefaultTextColor);
	}
}


void UTServerRow::ServerRowSelectButtonClicked()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->SetSelectedServerRowIndex(Index);
	}

	SetServerRowTextColor(SelectedTextColor);
}


void UTServerRow::ServerRowSelectButtonHovered()
{
	if (!bIsSelected)
	{
		SetServerRowTextColor(HoveredTextColor);
	}
}


void UTServerRow::ServerRowSelectButtonUnhovered()
{
	if (!bIsSelected)
	{
		SetServerRowTextColor(DefaultTextColor);
	}
}


void UTServerRow::SetServerRowTextColor(FSlateColor Color)
{
	if (ServerRowName)
	{
		ServerRowName->SetColorAndOpacity(Color);
	}

	if (ServerRowHost)
	{
		ServerRowHost->SetColorAndOpacity(Color);
	}

	if (ServerRowPlayers)
	{
		ServerRowPlayers->SetColorAndOpacity(Color);
	}
}
