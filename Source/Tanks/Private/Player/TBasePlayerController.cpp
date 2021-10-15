// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TBasePlayerController.h"

// Engine Includes
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"


// Game Includes
#include "Player/TBasePlayerState.h"
#include "Pawns/TPlayerTank.h"
#include "GameModes/TBaseGameMode.h"
#include "UMG/TPlayerTankMainWidget.h"

#include "UMG/TMultiplayerScoreBoard.h"
#include "TEventGameStateChange.h"
#include "Weapons/TShootableWeapon.h"

void ATBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Setup Widgets
	if (IsLocalPlayerController())
	{
		/*if (PlayerTankMainWidgetClass)
		{
			PlayerHUD = CreateWidget<UTPlayerTankMainWidget>(this, PlayerTankMainWidgetClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToViewport();
				PlayerHUD->ResetMainWidget();
			}
		}*/

		/*if (MultiplayerScoreBoardWidgetClass)
		{
			MultiplayerScoreBoard = CreateWidget<UTMultiplayerScoreBoard>(this, MultiplayerScoreBoardWidgetClass);
		}*/
	}

	// Pawn control will be enabled once match starts (ClientStartMatchStartCountDownTimer)
	SetEnablePlayerInputToPawn(bPawnControlEnabledOnStart);

	TeamId = FGenericTeamId(51);

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
		if (GI)
		{
			UTMWGlobalEventHandler* EventHandlerSubsystem = GI->GetSubsystem<UTMWGlobalEventHandler>();
			if (EventHandlerSubsystem)
			{
				MyBoundEvent.BindDynamic(this, &ATBasePlayerController::OnGameStateEventReceived);
				EventHandlerSubsystem->BindGlobalEventByClass(UTEventGameStateChange::StaticClass(), MyBoundEvent);
			}
		}

		OnDestroyed.AddDynamic(this, &ATBasePlayerController::OnPlayerControllerDestroyed);
	}

	DamageDisplayCounter = 0;
}




void ATBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("ShowScoreBoard"), EInputEvent::IE_Pressed, this, &ATBasePlayerController::OnShowScoreBoard);
	InputComponent->BindAction(TEXT("ShowScoreBoard"), EInputEvent::IE_Released, this, &ATBasePlayerController::OnHideScoreBoard);
}


void ATBasePlayerController::SetEnablePlayerInputToPawn(bool bEnabled)
{
	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		if (bEnabled)
		{
			MyPawn->EnableInput(this);
		}
		else
		{
			MyPawn->DisableInput(this);
		}
	}
}


void ATBasePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	ClientResetPlayerHUD();

	if (IsLocalPlayerController())
	{
		if (PlayerTankMainWidgetClass)
		{
			if (PlayerHUD)
			{
				PlayerHUD->BeginDestroy();
			}

			PlayerHUD = CreateWidget<UTPlayerTankMainWidget>(this, PlayerTankMainWidgetClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToViewport();
				PlayerHUD->ResetMainWidget();
			}
		}

		ATPlayerTank* LocalPlayerTank = Cast<ATPlayerTank>(aPawn);
		if (LocalPlayerTank && PlayerHUD)
		{
			//PlayerHUD->SetLocalPlayerTank(LocalPlayerTank);
		}
	}


}


void ATBasePlayerController::MyPawnHealthChange(AController* DamagingController, float CurrentHealth, float HealthDelta, bool bMyPawnDied)
{	
	bool bHasAuthority = GetLocalRole() == ENetRole::ROLE_Authority ? true : false;

	// Health Delta less then 0 are damage
	if (bHasAuthority && HealthDelta < 0.f)
	{
		UpdateGameModeForPlayerDamage(DamagingController, FMath::Abs(HealthDelta), bMyPawnDied);
	}

	// Update If pawn died tell PlayerHUD to show killer
	if (bMyPawnDied)
	{
		FString KillerName = "";
		APlayerController* PC = Cast<APlayerController>(DamagingController);
		if (PC)
		{
			APlayerState* PS = PC->PlayerState;
			if (PS)
			{
				KillerName = PS->GetPlayerName();
			}
		}

		if (bHasAuthority && IsLocalPlayerController())
		{
			//ClientUpdatePlayerHUDDeath_Implementation(KillerName);
		}
		else
		{
			//ClientUpdatePlayerHUDDeath(KillerName);
		}
	}
	else
	{
		if (bHasAuthority && IsLocalPlayerController())
		{
			//ClientUpdatePlayerHUDHealth_Implementation(CurrentHealth);
		}
		else
		{
			//ClientUpdatePlayerHUDHealth(CurrentHealth);
		}
	}
}




void ATBasePlayerController::UpdateGameModeForPlayerDamage(AController* InstigatorController, float Damage, bool bMyPawnDied)
{
	// Update PlayerScore via GameMode
	auto GM = GetWorld()->GetAuthGameMode();
	if (GM)
	{
		auto BaseGameMode = Cast<ATBaseGameMode>(GM);
		if (BaseGameMode)
		{
			BaseGameMode->AssignPlayerDamageScore(InstigatorController, this, Damage, bMyPawnDied);
		}
	}
}



void ATBasePlayerController::ClientResetPlayerHUD_Implementation()
{
	if (PlayerHUD)
	{
		PlayerHUD->ResetMainWidget();
	}
}


void ATBasePlayerController::OnShowScoreBoard()
{
	if (MultiplayerScoreBoard)
	{
		MultiplayerScoreBoard->ShowScoreBoard(true);
	}
}


void ATBasePlayerController::OnHideScoreBoard()
{
	if (MultiplayerScoreBoard)
	{
		MultiplayerScoreBoard->ShowScoreBoard(false);
	}
}


void ATBasePlayerController::DisplayDamageCaused(AActor* DamagedActor, float Damage)
{
	// Damage is only computed on Server
	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	if (!DamagedActor) return;

	DamageDisplay.Damage = Damage;
	DamageDisplay.DamageCausedLocation = DamagedActor->GetActorLocation();
	DamageDisplay.DamageCausedCounter = DamageDisplayCounter++;

	if (GetLocalRole() == ENetRole::ROLE_Authority && IsLocalPlayerController())
	{
		OnRep_DamageDisplay();
	}
}

void ATBasePlayerController::OnRep_DamageDisplay()
{
	SpawnDamageDisplayWidget(DamageDisplay.DamageCausedLocation, DamageDisplay.Damage);
}


void ATBasePlayerController::OnGameStateEventReceived(UObject* Publisher, UObject* Payload, const TArray<FString>& Metadata)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMyCppEventReceived"));
	
	UTEventGameStateChange* CurrentGameState = Cast<UTEventGameStateChange>(Payload);
	if (CurrentGameState)
	{
		int32 StateTime = CurrentGameState->StateTimeSeconds;
		const FString StateMessage = CurrentGameState->StateMessage;

		switch (CurrentGameState->NewGameState)
		{
			case EGameInProgressState::EGINP_WaitingForInProgress:
				UE_LOG(LogTemp, Warning, TEXT("OnGameStateEventReceived, State = EGINP_WaitingForInProgress, StateTime = %d"), CurrentGameState->StateTimeSeconds);

			break;

			case  EGameInProgressState::EGINP_StartPlayerDelay:
				UE_LOG(LogTemp, Warning, TEXT("OnGameStateEventReceived, State = EGINP_StartPlayerDelay, StateTime = %d"), CurrentGameState->StateTimeSeconds);
				ClientWaitingForPlayers(StateTime, StateMessage);
			break;

			case EGameInProgressState::EGINP_GameStartCountDown:
				UE_LOG(LogTemp, Warning, TEXT("OnGameStateEventReceived, State = EGINP_GameStartCountDown, StateTime = %d"), CurrentGameState->StateTimeSeconds);
				ClientStartMatchStartCountDownTimer(StateTime, StateMessage);
			break;

			case EGameInProgressState::EGINP_GameInProgress:
				UE_LOG(LogTemp, Warning, TEXT("OnGameStateEventReceived, State = EGINP_GameInProgress, StateTime = %d"), CurrentGameState->StateTimeSeconds);
				ClientStartMatchInProgress(StateTime, StateMessage);
			break;

			case EGameInProgressState::EGINP_GameTimeExpired:
				UE_LOG(LogTemp, Warning, TEXT("OnGameStateEventReceived, State = EGINP_GameTimeExpired, StateTime = %d"), CurrentGameState->StateTimeSeconds);
				ClientEndMatchInProgress(StateTime, StateMessage);
			break;

			default:
			break;
		}
	}
}


void ATBasePlayerController::ClientWaitingForPlayers_Implementation(int32 TimeToWait, const FString& Message)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetTopStatusMessage(FText::FromString(Message));
		PlayerHUD->ClearStatusText(false, true);

		// PlayerHUD->ShowStatusText(ESelectedStatus::ESS_TopStatsTextBox, Message);
		// PlayerHUD->ClearStatusText(ESelectedStatus::ESS_BottomStatusTextBox);
	}
}


void ATBasePlayerController::ClientStartMatchStartCountDownTimer_Implementation(int32 CountDownSeconds, const FString& Message)
{
	if (PlayerHUD)
	{
		PlayerHUD->StartMatchStartCountDown(FText::FromString(Message), CountDownSeconds);
	}

	FTimerHandle TimerHandle_EnalbeInput;
	FTimerDelegate EnablePlayerInputDelegate = FTimerDelegate::CreateUObject(this, &ATBasePlayerController::SetEnablePlayerInputToPawn, true);
	GetWorldTimerManager().SetTimer(TimerHandle_EnalbeInput, EnablePlayerInputDelegate, CountDownSeconds, false);
}


void ATBasePlayerController::ClientStartMatchInProgress_Implementation(int32 MatchDurationSeconds, const FString& Message)
{
	//if (PlayerHUD)
	//{
	//	PlayerHUD->ClearStatusText(ESelectedStatus::ESS_TopStatsTextBox);
	//	PlayerHUD->ClearStatusText(ESelectedStatus::ESS_BottomStatusTextBox);

	//	if (MatchDurationSeconds >= 0)
	//	{
	//		PlayerHUD->StartTimeRemainingCountDown(MatchDurationSeconds);
	//	}
	//	else
	//	{
	//		// TODO: Hide Time
	//	}
	//}

	SetEnablePlayerInputToPawn(true);
}


void ATBasePlayerController::ClientEndMatchInProgress_Implementation(int32 TimeLeftTillTravelSeconds, const FString& Message)
{
	if (MultiplayerScoreBoard)
	{
		MultiplayerScoreBoard->ShowScoreBoard(true);
		MultiplayerScoreBoard->bCanToggleShowBoard = false;
		MultiplayerScoreBoard->SetScoreBoardTitleText(TEXT("Final Score"));
	}
}


void ATBasePlayerController::OnPlayerControllerDestroyed(AActor* Actor)
{
	

}

FGenericTeamId ATBasePlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void ATBasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION(ATBasePlayerController, DamageDisplay, COND_OwnerOnly);


}
