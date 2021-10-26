// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TLobbyPlayerState.generated.h"

/**
 * Player State for multiplayer lobby map
 */
UCLASS()
class TANKS_API ATLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

/**
 * Members
 */
 private:

	/** Player has voted to start the match */
	UPROPERTY(Replicated)
	bool bVoteToStart = false;


 /**
  * Methods
  */

public:

	/** Set local player is ready or not ready to start match */
	void SetVoteToStart(bool Value);

	UFUNCTION(Server, Reliable)
	void ServerSetVoteToStart(bool Value);

	bool GetVoteToStart() const { return bVoteToStart; }

};
