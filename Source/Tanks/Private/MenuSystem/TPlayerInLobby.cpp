// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/TPlayerInLobby.h"

// Engine Includes
#include "Components/TextBlock.h"

void UTPlayerInLobby::SetPlayerName(FText PlayerName)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(PlayerName);
	}
}

