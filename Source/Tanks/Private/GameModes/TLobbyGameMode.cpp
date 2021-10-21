// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TLobbyGameMode.h"

// Engine Includes
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"


// Game Includes
#include "GameStates/TLobbyGameState.h"
#include "Player/TLobbyPlayerState.h"

ATLobbyGameMode::ATLobbyGameMode()
{
	bNextMapLoadTimerSet = false;
	WaitTimeAfterIsReadyToStart = 5.f;
	PlayerCount = 0;
}




void ATLobbyGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

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
		--WaitTimeAfterIsReadyToStart;
		CurrentLobbyData.TimeTillMatchStart = WaitTimeAfterIsReadyToStart;
		if (WaitTimeAfterIsReadyToStart <= 0.f)
		{
			LoadNextMap();
		}
	}
	else
	{
		CurrentLobbyData.ServerName = GetSessionServerName();
		CurrentLobbyData.MaxPlayers = GetSessionMaxPlayers();
		CurrentLobbyData.CurrentPlayers = PlayerCount;	//GetSessionCurrentPlayers();
		CurrentLobbyData.VotesToStart = GetTotalVotesToStart();
		CurrentLobbyData.VotesNeededToStart = FMath::FloorToInt(CurrentLobbyData.MaxPlayers / 2.f);

		// If more then half the players are ready to then start match
		CurrentLobbyData.bIsMatchStarting = IsReadyToStart(CurrentLobbyData.VotesToStart, CurrentLobbyData.MaxPlayers);
		if (CurrentLobbyData.bIsMatchStarting)
		{
			CurrentLobbyData.TimeTillMatchStart = WaitTimeAfterIsReadyToStart;
		}
	}

	LobbyGameState->SetLobbyData(CurrentLobbyData);
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


int32 ATLobbyGameMode::GetSessionMaxPlayers() const
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		FNamedOnlineSession* NamedOnlineSession = SessionInterface->GetNamedSession(GameSessionName);
		if (NamedOnlineSession)
		{
			return NamedOnlineSession->SessionSettings.NumPublicConnections;
		}
	}

	return -1;
}

FString ATLobbyGameMode::GetSessionServerName() const
{
	FString SessionName = "";
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		FNamedOnlineSession* NamedOnlineSession = SessionInterface->GetNamedSession(GameSessionName);
		if (NamedOnlineSession)
		{
			NamedOnlineSession->SessionSettings.Get(SETTING_MAPNAME, SessionName);
		}
	}

	return SessionName;
}


int32 ATLobbyGameMode::GetSessionCurrentPlayers() const
{
	ATLobbyGameState* LobbyGameState = GetGameState<ATLobbyGameState>();
	if (LobbyGameState)
	{
		return LobbyGameState->PlayerArray.Num();
	}

	return -1;
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
	return (VotesToStart >= (MaxPlayers / 2));
}


void ATLobbyGameMode::LoadNextMap()
{
	UWorld* World = GetWorld();
	if (!World) return;

	bUseSeamlessTravel = true;

	World->ServerTravel("/Game/Tanks/Map/Map_WW2_Test?listen");
}
