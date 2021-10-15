// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TLobbyPlayerController.generated.h"

class UTTanksGameInstance;
class UTMultiplayerLobbyMenu;

/**
 * 
 */
UCLASS()
class TANKS_API ATLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()


/**
 * Members
 */

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LobbyMenu")
	TSubclassOf<UTMultiplayerLobbyMenu> LobbyMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="LobbyMenu")
	UTMultiplayerLobbyMenu* LobbyMenu;


/**
* Methods
*/

private:

	void SpawnLobbyWidget();


protected:

	/** Vote to Start on Server RCP */
	UFUNCTION(Server, Reliable)
	void ServerVoteToStart(bool bStart);

	virtual void BeginPlay() override;




public:

	/** Vote to start the match */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void VoteToStart(bool bStart);

	void UpdateLobbyMenu() const;

};
