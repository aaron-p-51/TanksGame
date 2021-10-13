// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TTanksLobbyGameMode.h"

// Engine Includes
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"

// Game Includes
#include "GameStates/TLobbyGameState.h"
#include "Player/TLobbyPlayerState.h"

#include "Player/TLobbyPlayerController.h"
#include "TTanksGameInstance.h"

#include "Subsystems/TSessionSubsystem.h"
#include "Containers/UnrealString.h"



ATTanksLobbyGameMode::ATTanksLobbyGameMode()
{
	bNextMapLoadTimerSet = false;
	WaitTimeAfterIsReadyToStart = 5.f;
}


void ATTanksLobbyGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//GetWorldTimerManager().SetTimer(TimerHandle_DefaultTick, this, &ATTanksLobbyGameMode::DefaultTick, 1.f, true);
	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTick, this, &ATTanksLobbyGameMode::DefaultTick, 1.0f, true, 2.0f);
}


void ATTanksLobbyGameMode::DefaultTick()
{
	// Set updated LobbyData
	FLobbyData LobbyData;



	// Check how many Votes to start
	int32 VotesToStart = GetTotalVotesToStart();

	// Get Session Name
	const IOnlineSessionPtr SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if (SessionInterface)
	{
		FOnlineSessionSettings* OnlineSessionSettings = SessionInterface->GetSessionSettings(GameSessionName);
		LobbyData.MaxPlayers = OnlineSessionSettings->NumPublicConnections;
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





	//auto GameInstance = GetGameInstance();
	//if (GameInstance)
	//{
	//auto GI = Cast<UTTanksGameInstance>(GameInstance);
	////UTTanksGameInstance* GI = GetGameInstance<UTTanksGameInstance>();
	//if (GI->IsValidLowLevel())
	//{
	//	FMultiplayerLobbyData MultiplayerLobbyData;
	//	//int32 MaxPlayers = GI->GetHostingSessionMaxPlayers();
	//	//MultiplayerLobbyData.MaxPlayers = GI->GetHostingSessionMaxPlayers();
	//	//MultiplayerLobbyData.ServerName = GI->GetHostingSessionServerName();
	//	MultiplayerLobbyData.VotesToStart = VotesToStart;
	//	MultiplayerLobbyData.bIsStarting = IsReadyToStart(VotesToStart, MultiplayerLobbyData.MaxPlayers);
	//	//MultiplayerLobbyData.bIsStarting = 

	//	// Only start TimerHandle_LoadNextMap once, this is used by player widget for count down timer 
	//	if (MultiplayerLobbyData.bIsStarting && !bNextMapLoadTimerSet)
	//	{
	//		bNextMapLoadTimerSet = true;
	//		MultiplayerLobbyData.TimeTillStartDuration = WaitTimeAfterIsReadyToStart;
	//		GetWorldTimerManager().SetTimer(TimerHandle_LoadNextMap, this, &ATTanksLobbyGameMode::LoadNextMap, WaitTimeAfterIsReadyToStart, false);
	//	}

	//	ATTanksLobbyGameState* GS = GetGameState<ATTanksLobbyGameState>();
	//	if (GS)
	//	{
	//		GS->SetMultiplayerLobbyData(MultiplayerLobbyData);
	//	}
	//}
	//}
}


int32 ATTanksLobbyGameMode::GetTotalVotesToStart()
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


bool ATTanksLobbyGameMode::IsReadyToStart(int32 VotesToStart, int32 MaxPlayers)
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


void ATTanksLobbyGameMode::LoadNextMap()
{
	GetWorld()->ServerTravel("/Game/Tanks/Maps/Map_WW2_Test?listen");
}

