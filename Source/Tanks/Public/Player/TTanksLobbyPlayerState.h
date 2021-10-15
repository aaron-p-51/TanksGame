// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTanksLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TANKS_API ATTanksLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

/**
 * Members
 */
protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
	bool bVoteToStart;


 /**
  * Methods
  */

public:

	ATTanksLobbyPlayerState();

	void SetVoteToStart(bool bValue);

	FORCEINLINE bool GetVoteToStart() const { return bVoteToStart; }
		
};
