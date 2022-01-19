// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TMultiplayerBaseGameMode.h"

#include "GameStates/TMultiplayerBaseGameState.h"


ATMultiplayerBaseGameMode::ATMultiplayerBaseGameMode()
{
	StartPlayersDelaySeconds = 3;
	GameStartCountDownSeconds = 3;
	GameInProgressSeconds = 180;	// 3 minutes
	GameExitCountDownSeconds = 5;
}


void ATMultiplayerBaseGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	SetInProgressStates();
	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ATMultiplayerBaseGameMode::DefaultTimerTick, 1.f, true);
}


void ATMultiplayerBaseGameMode::SetInProgressStates()
{
	InProgressStates.Add(FTanksGameInProgressState(EGameInProgressState::EGINP_WaitingForInProgress, 0));

	if (StartPlayersDelaySeconds > 0)
	{
		InProgressStates.Add(FTanksGameInProgressState(EGameInProgressState::EGINP_StartPlayerDelay, StartPlayersDelaySeconds));
	}
	if (GameStartCountDownSeconds > 0)
	{
		InProgressStates.Add(FTanksGameInProgressState(EGameInProgressState::EGINP_GameStartCountDown, GameStartCountDownSeconds));
	}
	if (GameInProgressSeconds > 0)
	{
		InProgressStates.Add(FTanksGameInProgressState(EGameInProgressState::EGINP_GameInProgress, GameInProgressSeconds));
	}
	if (GameExitCountDownSeconds > 0)
	{
		InProgressStates.Add(FTanksGameInProgressState(EGameInProgressState::EGINP_GameExitCountDown, GameExitCountDownSeconds));
	}

	InProgressStates.Add(FTanksGameInProgressState(EGameInProgressState::EGINP_GameTimeExpired, 0));
}


void ATMultiplayerBaseGameMode::DefaultTimerTick()
{
	if (GetMatchState() == MatchState::InProgress)
	{
		ATMultiplayerBaseGameState* MultiplayerBaseGameState = Cast<ATMultiplayerBaseGameState>(GameState);
		if (MultiplayerBaseGameState)
		{
			// Only process state if state is between WaitingForInProgress and GameTimeExpired exclusively
			FTanksGameInProgressState GameInProgressState = MultiplayerBaseGameState->GetGameInProgressState();
			if (GameInProgressState.State != EGameInProgressState::EGINP_WaitingForInProgress &&
				GameInProgressState.State != EGameInProgressState::EGINP_GameTimeExpired)
			{
				if (GameInProgressState.RemainingSeconds > 0)
				{
					MultiplayerBaseGameState->DecrementGameInProgressStateTime();
					if (MultiplayerBaseGameState->GetGameInProgressState().RemainingSeconds <= 0)
					{
						bool StateChangeSuccess = TryChangeToNextGameInProgressState();

						if (MultiplayerBaseGameState->GetGameInProgressState().State == EGameInProgressState::EGINP_GameTimeExpired)
						{
							TravelToLobby();
						}
					}
				}
				if (GameInProgressState.State == EGameInProgressState::EGINP_GameInProgress)
				{
					RestartDeadPlayers();
				}
			}
		}
	}
}


void ATMultiplayerBaseGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	
	AGameState* GS = GetGameState<AGameState>();
	if (GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("InProgress: Current Player Count = %d"), GS->PlayerArray.Num());
	}

	TryChangeToNextGameInProgressState();
}


bool ATMultiplayerBaseGameMode::TryChangeToNextGameInProgressState()
{
	if (GetMatchState() != MatchState::InProgress)
	{
		UE_LOG(LogTemp, Error, TEXT("ChangeGameInProgressState called when MatchState != MatchState::InProgress"));
		return false;
	}

	++InProgressStatesIndex;
	if (InProgressStatesIndex < InProgressStates.Num())
	{
		ATMultiplayerBaseGameState* MultiplayerBaseGameState = Cast<ATMultiplayerBaseGameState>(GameState);
		if (MultiplayerBaseGameState)
		{
			return MultiplayerBaseGameState->SetNextGameInProgressState(InProgressStates[InProgressStatesIndex]);
		}
	}

	return false;
}


void ATMultiplayerBaseGameMode::TravelToLobby()
{
	bUseSeamlessTravel = true;

	GetWorld()->ServerTravel("/Game/Tanks/Map/MultiplayerLobby_New?listen");
}

