// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TLobbyGameMode.h"

// Engine Includes
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"


// Game Includes
#include "GameStates/TLobbyGameState.h"
#include "Player/TLobbyPlayerState.h"


ATLobbyGameMode::ATLobbyGameMode()
{
	WaitTimeAfterIsReadyToStart = 5.f;
	PlayerCount = 0;
}


void ATLobbyGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// Set DefaultTick() to be called every second
	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTick, this, &ATLobbyGameMode::DefaultTick, 1.0f, true, 2.0f);
}


void ATLobbyGameMode::DefaultTick()
{
	// Get the current game state
	ATLobbyGameState* LobbyGameState = GetGameState<ATLobbyGameState>();
	if (!LobbyGameState) return;
	FLobbyData CurrentLobbyData = LobbyGameState->GetLobbyData();

	if (CurrentLobbyData.bIsMatchStarting)
	{
		// Match start criteria has been meet. CurrentLobbyData is passed as reference and will be modified by ProcessLoadNextMap
		ProcessLoadNextMap(CurrentLobbyData);
	}
	else
	{
		// Update Lobby data based on current session info. CurrentLobbyData is passed as reference and will be modified by ProcessLoadNextMap
		UpdateLobbyData(CurrentLobbyData);
	}

	LobbyGameState->SetLobbyData(CurrentLobbyData);
}


void ATLobbyGameMode::ProcessLoadNextMap(FLobbyData& LobbyData)
{
	--WaitTimeAfterIsReadyToStart;
	LobbyData.TimeTillMatchStart = WaitTimeAfterIsReadyToStart;

	// Load the next max when WaitTimeAfterIsReadyToStart elapses
	if (WaitTimeAfterIsReadyToStart <= 0.f)
	{
		LoadNextMap();
	}
}


void ATLobbyGameMode::UpdateLobbyData(FLobbyData& LobbyData)
{
	// Update Session Name and Max Players
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		FNamedOnlineSession* NamedOnlineSession = SessionInterface->GetNamedSession(GameSessionName);
		LobbyData.ServerName = GetSessionServerName(NamedOnlineSession);
		LobbyData.MaxPlayers = GetSessionMaxPlayers(NamedOnlineSession);
	}

	LobbyData.CurrentPlayers = PlayerCount;
	LobbyData.VotesToStart = GetTotalVotesToStart();

	// If more then half the players are ready to then start match
	LobbyData.VotesNeededToStart = FMath::FloorToInt(LobbyData.MaxPlayers / 2.f);
	LobbyData.bIsMatchStarting = IsReadyToStart(LobbyData.VotesToStart, LobbyData.MaxPlayers);

	if (LobbyData.bIsMatchStarting)
	{
		LobbyData.TimeTillMatchStart = FMath::Max<float>(0.f, WaitTimeAfterIsReadyToStart);
	}
}


FString ATLobbyGameMode::GetSessionServerName(FNamedOnlineSession* NamedOnlineSession) const
{
	FString SessionName = "";
	if (NamedOnlineSession)
	{
		NamedOnlineSession->SessionSettings.Get(SETTING_MAPNAME, SessionName);
	}

	return SessionName.ToUpper();
}


int32 ATLobbyGameMode::GetSessionMaxPlayers(FNamedOnlineSession* NamedOnlineSession) const
{
	if (!NamedOnlineSession) return -1;
	return NamedOnlineSession->SessionSettings.NumPublicConnections;
}


void ATLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++PlayerCount;
}


void ATLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	PlayerCount = FMath::Max<int32>(0, --PlayerCount);
}


int32 ATLobbyGameMode::GetTotalVotesToStart() const
{
	int32 VotesToStart = 0;

	// Check each PlayerState via each PlayerController and check if they have voted to start the next match. 
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
	return (VotesToStart >= (MaxPlayers / 2));
}


void ATLobbyGameMode::LoadNextMap()
{
	UWorld* World = GetWorld();
	if (!World) return;

	bUseSeamlessTravel = true;

	World->ServerTravel("/Game/Tanks/Map/Map_WW2_Test?listen");
}
