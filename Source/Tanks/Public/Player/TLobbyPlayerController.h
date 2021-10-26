// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TLobbyPlayerController.generated.h"

/** Forward declarations */
class UTTanksGameInstance;
class UTMultiplayerLobbyMenu;

/**
 * Player controller for multiplayer lobby map
 */
UCLASS()
class TANKS_API ATLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()


/**
* Methods
*/

public:

	/** Vote to start the match */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void VoteToStart(bool bStart);

	/** Vote to Start on Server RCP */
	UFUNCTION(Server, Reliable)
	void ServerVoteToStart(bool bStart);

};
