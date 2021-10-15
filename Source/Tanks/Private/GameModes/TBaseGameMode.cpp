// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/TBaseGameMode.h"

#include "Player/TBasePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "TMWGlobalEventHandler.h"
#include "TEventGameStateChange.h"


#include "Player/TBasePlayerState.h"
#include "GameStates/TBaseGameState.h"
#include "TSpawnLocation.h"
#include "Pawns/TPlayerTank.h"


ATBaseGameMode::ATBaseGameMode()
{
	PlayerStateClass = ATBasePlayerState::StaticClass();
	GameStateClass = ATBaseGameState::StaticClass();

	GameInProgressState = EGameInProgressState::EGINP_WaitingForInProgress;
	
	bUseStartPlayersDelay = true;
	StartPlayersDelaySeconds = 1;
	bNextStateIfAllPlayers = true;

	bUseGameStartCountDown = true;
	GameStartCountDownSeconds = 3;

	bUseGameTime = true;
	GameTimeSeconds = 300;

	bUseGameTimeToExit = true;
	GameTimeToExitSeconds = 3;

	bDelayedStart = true;

	UE_LOG(LogTemp, Warning, TEXT("In ATBaseGameMode, MatchState = %s"), *GetMatchState().ToString());

	GameInProgressState = EGameInProgressState::EGINP_WaitingForInProgress;

	GameMyStateChange = NewObject<UTEventGameStateChange>();
}




void ATBaseGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ATBaseGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}


//void ATBaseGameMode::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//
//	switch (GameInProgressState)
//	{
//	case EGameInProgressState::EGINP_WaitingForInProgress:
//		break;
//	case EGameInProgressState::EGINP_WaitingForPlayersToSpawn:
//		break;
//	case EGameInProgressState::EGINP_MatchInProgress:
//		break;
//	case EGameInProgressState::EGINP_MatchTimeExpired:
//		break;
//	default:
//		break;
//	}
//
//	//SpawnDefaultPawnAtTransform(NewPlayer, SpawnLocations[0]->GetActorTransform());
//}

void ATBaseGameMode::DefaultTimer()
{
	if (GetMatchState() != MatchState::InProgress || GameInProgressState == EGameInProgressState::EGINP_WaitingForInProgress) return;

	ATBaseGameState* BaseGameState = Cast<ATBaseGameState>(GameState);
	if (BaseGameState && BaseGameState->RemainingTime > 0)
	{
		BaseGameState->RemainingTime--;
		UE_LOG(LogTemp, Warning, TEXT("DefaultTimer(): Remaining Time = %d"), BaseGameState->RemainingTime);

		if (GameInProgressState == EGameInProgressState::EGINP_StartPlayerDelay && bNextStateIfAllPlayers)
		{
			CheckIfReadyForGameInProgressState();
		}

		if (BaseGameState->RemainingTime <= 0)
		{
			if (GameInProgressState == EGameInProgressState::EGINP_StartPlayerDelay)
			{
				ChangeToGameStartCountDown();
			}
			else if (GameInProgressState == EGameInProgressState::EGINP_GameStartCountDown)
			{
				ChangeToGameInProgress();
			}
			else if (GameInProgressState == EGameInProgressState::EGINP_GameInProgress)
			{
				ChangeToGameTimeExpired();
			}
			else if (GameInProgressState == EGameInProgressState::EGINP_GameTimeExpired)
			{
				TravelBackToLobby();
			}

		}
	}

	// TODO: One Classes are derived from this class make StartPlayers pure virtual
	/*if (GameInProgressState > EGameInProgressState::EGINP_WaitingForPlayersToSpawn)
	{
		ATBaseGameState* const MyGameState = Cast<ATBaseGameState>(GameState);
		if (MyGameState && MyGameState->RemainingTime > 0)
		{
			MyGameState->RemainingTime--;

			if (MyGameState->RemainingTime <= 0)
			{
				if (GetMatchState() == MatchState::InProgress)
				{
					ChangeGameInProgressState(EGameInProgressState::EGINP_MatchTimeExpired);
				}
				else if (GetMatchState() == MatchState::WaitingPostMatch)
				{
					TravelBackToLobby();
				}
			}

			RespawnDeadPlayers();
		}
	}*/
}







void ATBaseGameMode::CheckIfReadyForGameInProgressState()
{
	if (NumTravellingPlayers <= 0)
	{
		ChangeToGameStartCountDown();
	}
}





void ATBaseGameMode::HandleMatchHasStarted()
{
	// HandleMatchHasStated is called when GameMode transitions from MatchState::WaitingToStart to MatchState::InProgress
	Super::HandleMatchHasStarted();

	UE_LOG(LogTemp, Warning, TEXT("In ATBaseGameMode::HandleMatchHasStarted"));

	ChangeToStartPlayerDelay();
}


void ATBaseGameMode::ChangeToStartPlayerDelay()
{	
	UE_LOG(LogTemp, Warning, TEXT("ChangeToStartPlayerDelay()"));

	if (bUseStartPlayersDelay)
	{
		ResetGameStateTimer(StartPlayersDelaySeconds);
		GameInProgressState = EGameInProgressState::EGINP_StartPlayerDelay;
		UE_LOG(LogTemp, Warning, TEXT("Set State: EGINP_StartPlayerDelay"));
		SendGameStateChangeEvent(GameInProgressState, StartPlayersDelaySeconds, StartPlayerDelayMessage);
	}
	else
	{
		ChangeToGameStartCountDown();
	}
}


void ATBaseGameMode::ChangeToGameStartCountDown()
{
	UE_LOG(LogTemp, Warning, TEXT("ChangeToGameStartCountDown()"));

	if (bUseGameStartCountDown)
	{
		ResetGameStateTimer(GameStartCountDownSeconds);
		GameInProgressState = EGameInProgressState::EGINP_GameStartCountDown;
		UE_LOG(LogTemp, Warning, TEXT("Set State: EGINP_GameStartCountDown"));
		SendGameStateChangeEvent(GameInProgressState, GameStartCountDownSeconds, GameStartCountDownMessage);
	}
	else
	{
		ChangeToGameInProgress();
	}
}


void ATBaseGameMode::ChangeToGameInProgress()
{
	UE_LOG(LogTemp, Warning, TEXT("ChangeToGameInProgress()"));
	
	if (bUseGameTime)
	{
		ResetGameStateTimer(GameTimeSeconds);
	}
	else
	{
		// If there is no time for the level then no need to call default timer anymore
		// call must be made to LevelComplete()
		// TODO: Add LevelComplete()
		GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);
	}

	GameInProgressState = EGameInProgressState::EGINP_GameInProgress;
	UE_LOG(LogTemp, Warning, TEXT("Set State: EGINP_GameInProgress"));

	// If there is no time limit for GameTime state
	if (!bUseGameTime)
	{
		GameTimeSeconds = -1;
	}
	SendGameStateChangeEvent(GameInProgressState, GameTimeSeconds, GameTimeMessage);
}


void ATBaseGameMode::ChangeToGameTimeExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("ChangeToGameTimeExpired()"));

	if (bUseGameTimeToExit)
	{
		ResetGameStateTimer(GameTimeToExitSeconds);
		GameInProgressState = EGameInProgressState::EGINP_GameTimeExpired;
		UE_LOG(LogTemp, Warning, TEXT("Set State: EGINP_GameTimeExpired"));
		SendGameStateChangeEvent(GameInProgressState, GameTimeToExitSeconds, GameTimeToExitMessage);
	}
	else
	{
		TravelBackToLobby();
	}
}


void ATBaseGameMode::ResetGameStateTimer(int32 Seconds)
{
	ATBaseGameState* BaseGameState = Cast<ATBaseGameState>(GameState);
	if (BaseGameState)
	{
		BaseGameState->RemainingTime = Seconds;
	}
}


//void ATBaseGameMode::StartPlayers()
//{
//	// TODO: One Classes are derived from this class make StartPlayers pure virtual
//
//	//GetWorldTimerManager().ClearTimer(TimerHandle_StartPlayersDelay);
//
//	//// Tell each PlayerController to prepare for match start. PlayerControllers will start a match count down timer
//	//// and when complete will enable input control to their pawns
//	//for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	//{
//	//	UE_LOG(LogTemp, Warning, TEXT("StartMatchFromGM"));
//	//	APlayerController* PC = It->Get();
//	//	if (PC)
//	//	{
//	//		auto BasePlayerController = Cast<ATBasePlayerController>(PC);
//	//		if (BasePlayerController)
//	//		{
//
//	//			BasePlayerController->ClientStartMatchStartCountDownTimer(MatchStartCountDownTime);
//	//		}
//	//	}
//	//}
//}

void ATBaseGameMode::SendGameStateChangeEvent(EGameInProgressState NewState, int32 StateDurationSeconds, const FString& StateMessage)
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (GI)
	{ 
		UTMWGlobalEventHandler* EventHandlerSubsystem = GI->GetSubsystem<UTMWGlobalEventHandler>();
		if (EventHandlerSubsystem)
		{
			TArray<FString> MetaData;
			GameMyStateChange->StateTimeSeconds = StateDurationSeconds;
			GameMyStateChange->NewGameState = NewState;
			GameMyStateChange->StateMessage = StateMessage;
			EventHandlerSubsystem->CallGlobalEventByClass(this, UTEventGameStateChange::StaticClass(), GameMyStateChange, MetaData);
		}
	}
}


void ATBaseGameMode::TravelBackToLobby()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Tanks/Maps/MultiplayerLobby?listen");
	}
}


//AActor* ATBaseGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName /*= TEXT("")*/)
//{
//	AActor* NewPlayerStart = ChoosePlayerStart_Implementation(Player);
//
//	return (NewPlayerStart != nullptr) ? NewPlayerStart : Super::FindPlayerStart_Implementation(Player, IncomingName);
//}


//AActor* ATBaseGameMode::ChoosePlayerStart_Implementation(AController* Player)
//{
//	APlayerStart* PlayerStart = nullptr;
//	if (GameInProgressState < EGameInProgressState::EGINP_MatchInProgress)
//	{
//		PlayerStart = ChoosePlayerStartPreMatch(Player);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("ChoosingDuringMatch"));
//		PlayerStart = ChoosePlayerStartDuringMatch(Player);
//	}
//
//	return (PlayerStart != nullptr) ? PlayerStart : Super::ChoosePlayerStart_Implementation(Player);
//}


//APlayerStart* ATBaseGameMode::ChoosePlayerStartPreMatch(AController* Player)
//{
//	// This is probably not the most efficient way to get player start pre match. 
//	// This will only run before players can control pawns so is probably not a big deal
//	//
//	// Match Player PlayerState in the PlayerArray stored in GameStateBase. Each PlayerStartTag has
//	// a numerical value. Based on index of the Player in the PlayerArray start at the corresponding
//	// PlayerStart with the tag that matches the index
//	auto Players = GetGameState<AGameStateBase>()->PlayerArray;
//	for (int32 i = 0; i < Players.Num(); ++i)
//	{
//		if (Player->GetPlayerState<APlayerState>() == Players[i])
//		{
//			for (TActorIterator<APlayerStart> PlayerStartIt(GetWorld()); PlayerStartIt; ++PlayerStartIt)
//			{
//				APlayerStart* SpawnPoint = *PlayerStartIt;
//				FString PlayerStartTag = SpawnPoint->PlayerStartTag.ToString();
//				if (PlayerStartTag.IsNumeric())
//				{
//					int32 spawnIndex = FCString::Atoi(*PlayerStartTag);
//					if (i == spawnIndex)
//					{
//						return SpawnPoint;
//					}
//				}
//			}
//		}
//	}
//
//	return nullptr;
//}


//APlayerStart* ATBaseGameMode::ChoosePlayerStartDuringMatch(AController* Player)
//{
//	TArray<APlayerStart*> PreferredSpawns;
//	TArray<APlayerStart*> FallbackSpawns;
//
//	APlayerStart* BestStart = nullptr;
//	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
//	{
//		APlayerStart* TestSpawn = *It;
//
//		// Check if spawn is allowed, (based on teams for future game modes)
//
//		if (IsSpawnPointPerferred(TestSpawn, Player))
//		{
//			PreferredSpawns.Add(TestSpawn);
//		}
//		else
//		{
//			FallbackSpawns.Add(TestSpawn);
//		}
//	}
//
//	if (PreferredSpawns.Num() > 0)
//	{
//		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
//		UE_LOG(LogTemp, Warning, TEXT("Using Prefered Spawn Location"));
//	}
//	else if (FallbackSpawns.Num() > 0)
//	{
//		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
//		UE_LOG(LogTemp, Warning, TEXT("Using Fallback Spawn Location"));
//	}
//
//	return BestStart;
//}


//bool ATBaseGameMode::IsSpawnPointPerferred(APlayerStart* SpawnPoint, AController* Player) const
//{
//	// Determine if spawn point is close to another player
//	if (!SpawnPoint || !Player)
//	{
//		return false;
//		UE_LOG(LogTemp, Warning, TEXT("NULL is spawnprefered"));
//	}
//
//	
//	auto BasePlayerController = Cast<ATBasePlayerController>(Player);
//		
//	const FVector SpawnLocation = SpawnPoint->GetActorLocation();
//
//	for (ATPlayerTank* OtherPlayer : TActorRange<ATPlayerTank>(GetWorld()))
//	{
//		if (OtherPlayer->GetController<ATBasePlayerController>() == BasePlayerController) continue;
//	
//		const FVector OtherPlayerLocation = OtherPlayer->GetActorLocation();
//		float DistanceToOtherPlayer = FVector::DistSquared(SpawnLocation, OtherPlayerLocation);
//		UE_LOG(LogTemp, Warning, TEXT("Distance to otherplayer Squared: %f"), DistanceToOtherPlayer);
//		if (DistanceToOtherPlayer > 100.f)
//		{
//				return true;
//		}
//
//	}
//		
//	return false;
//}


float ATBaseGameMode::AssignPlayerDamageScore(AController* InstigatorController, AController* VictimController, float DamageAmount, bool bVictimPawnDied)
{
	if (!InstigatorController || !VictimController || DamageAmount == 0.0f) return 0.f;

	float DamageScore = DamageAmount * PointsPerPlayerDamage;
	if (bVictimPawnDied)
	{
		DamageScore += PointsPerPlayerKill;
	}

	UE_LOG(LogTemp, Warning, TEXT("DamageScoreL %f"), DamageScore);

	// Update InstigatorPlayer Score
	if (DamageScore > 0.f)
	{
		auto InstigatorPlayerState = InstigatorController->GetPlayerState<ATBasePlayerState>();
		if (InstigatorPlayerState)
		{
			InstigatorPlayerState->UpdatePlayerScoreStats(DamageScore, bVictimPawnDied, false);
		}
	}

	// Update DamagedPlayer Score
	if (bVictimPawnDied)
	{
		auto DamagedPlayerState = VictimController->GetPlayerState<ATBasePlayerState>();
		if (DamagedPlayerState)
		{
			DamagedPlayerState->UpdatePlayerScoreStats(0, false, true);
		}
	}

	return DamageScore;
}


bool ATBaseGameMode::DamageValidInCurrentGameInProgressState()
{
	return true;
}


//void ATBaseGameMode::RespawnDeadPlayers()
//{
//	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		APlayerController* PC = It->Get();
//		if (PC && PC->GetPawn() == nullptr)
//		{
//			RestartPlayer(PC);
//			UE_LOG(LogTemp, Warning, TEXT("Here Respon"));
//		}
//	}
//}








