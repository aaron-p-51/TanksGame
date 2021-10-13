// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TLobbyGameMode.h"

// Engine Includes
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"


// Game Includes
#include "GameStates/TLobbyGameState.h"
#include "Player/TLobbyPlayerState.h"

ATLobbyGameMode::ATLobbyGameMode()
{
	bNextMapLoadTimerSet = false;
	WaitTimeAfterIsReadyToStart = 5.f;
}


void ATLobbyGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTick, this, &ATLobbyGameMode::DefaultTick, 1.0f, true, 2.0f);
}


void ATLobbyGameMode::DefaultTick()
{
	// Set updated LobbyData
	FLobbyData LobbyData;


	// Check how many Votes to start
	int32 VotesToStart = GetTotalVotesToStart();

	// Get Session Name
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		FNamedOnlineSession* NamedOnlineSession = SessionInterface->GetNamedSession(GameSessionName);
		if (NamedOnlineSession)
		{
			LobbyData.MaxPlayers = NamedOnlineSession->SessionSettings.NumPublicConnections;
			LobbyData.CurrentPlayers = LobbyData.MaxPlayers - NamedOnlineSession->NumOpenPublicConnections;

			FString SessionName;
			NamedOnlineSession->SessionSettings.Get(SETTING_MAPNAME, SessionName);
			LobbyData.ServerName = SessionName;
		}
	}

	ATLobbyGameState* LobbyGameState = GetGameState<ATLobbyGameState>();
	if (LobbyGameState)
	{
		LobbyData.CurrentPlayers = LobbyGameState->PlayerArray.Num();

		LobbyGameState->SetLobbyData(LobbyData);

		if (GEngine)
		{
			FString PublicConnectionString = FString::Printf(TEXT("NumpublicConnection: %d"), LobbyData.CurrentPlayers);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, PublicConnectionString);

			FString MaxConnectionString = FString::Printf(TEXT("NumpublicConnection: %d"), LobbyData.MaxPlayers);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, MaxConnectionString);
		}
	}
}


int32 ATLobbyGameMode::GetTotalVotesToStart() const
{
	int32 VotesToStart = 0;

	for (auto PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		APlayerController* PlayerController = PlayerControllerIterator->Get();
		if (PlayerController)
		{
			ATLobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<ATLobbyPlayerState>();
			if (LobbyPlayerState && LobbyPlayerState->GetVoteToStart())
			{
				++VotesToStart;
			}
		}
	}

	return VotesToStart;
}


bool ATLobbyGameMode::IsReadyToStart(const int32 VotesToStart, const int32 MaxPlayers) const
{
	int32 PlayerCount = 0;
	if (VotesToStart >= MaxPlayers / 2)
	{
		return true;
	}

	auto GS = GetGameState<AGameState>();
	if (GS)
	{
		PlayerCount = GS->PlayerArray.Num();
		if (PlayerCount >= MaxPlayers)
		{
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("IsReadyToStart: VotesToStart=%d, MaxPlayers=%d, PlayerCount=%d"), VotesToStart, MaxPlayers, PlayerCount);

	return false;
}


void ATLobbyGameMode::LoadNextMap()
{

}
