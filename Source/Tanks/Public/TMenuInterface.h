// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TANKS_API ITMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Host(FString ServerName, int32 PlayerCount) = 0;
	virtual void Join(uint32 Index) = 0;
	virtual void LoadMainMenuWidget() = 0;
	virtual void LoadInGameMenuWidget() = 0;
	virtual void LoadMultiplayerLobbyWidget() = 0;
	virtual void RefreshServerList() = 0;
	virtual void ExitFromCurrentGame() = 0;

};
