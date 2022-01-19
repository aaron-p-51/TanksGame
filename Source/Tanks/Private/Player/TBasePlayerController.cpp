// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TBasePlayerController.h"

// Engine Includes
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"


// Game Includes
#include "Player/TMultiplayerBasePlayerState.h"
#include "Pawns/TPlayerTank.h"
#include "UMG/TPlayerTankMainWidget.h"
#include "GameModes/TMultiplayerBaseGameMode.h"
#include "GameStates/TMultiplayerBaseGameState.h"
#include "../Components/THealthComponent.h"
#include "UMG/TMultiplayerScoreBoard.h"
#include "Weapons/TShootableWeapon.h"


ATBasePlayerController::ATBasePlayerController()
{
	bEnablePawnControl = false;
}

/**************************************************************************/
/* Setup */
/**************************************************************************/
void ATBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	BindToGameStateStateChangeEvents();
	CreatePlayerHUDWidget();
	CreateScoreWidget();

	TeamId = FGenericTeamId(51);

	bEnablePawnControl = false;
}


void ATBasePlayerController::BindToGameStateStateChangeEvents()
{
	ATMultiplayerBaseGameState* GameState = GetWorld()->GetGameState<ATMultiplayerBaseGameState>();
	if (GameState)
	{
		GameState->OnGameInProgress.AddDynamic(this, &ATBasePlayerController::OnGameInProgressStateChange);
		GameState->OnGameExitCountDown.AddDynamic(this, &ATBasePlayerController::OnGameExitCountdownStateChange);
	}
}


void ATBasePlayerController::OnGameInProgressStateChange(int32 StateTimeSeconds)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		SetControlledPawnCanTakeDamage(true);
		bEnablePawnControl = true;
		OnRep_EnablePawnControl();
	}
}


void ATBasePlayerController::OnGameExitCountdownStateChange(int32 StateTimeSeconds)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		SetControlledPawnCanTakeDamage(false);
	}

	if (IsLocalPlayerController())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnGameExitCountdownStateChange: PC"));
		if (MultiplayerScoreBoard)
		{
			MultiplayerScoreBoard->ShowScoreBoard(true);
			MultiplayerScoreBoard->bCanToggleShowBoard = false;
		}

		if (PlayerTankMainWidget)
		{
			PlayerTankMainWidget->RemoveFromViewport();
		}
	}
}


void ATBasePlayerController::CreatePlayerHUDWidget()
{
	// Setup Widgets
	if (IsLocalPlayerController() && PlayerTankMainWidgetClass && !PlayerTankMainWidget)
	{
		PlayerTankMainWidget = CreateWidget<UTPlayerTankMainWidget>(this, PlayerTankMainWidgetClass);
		if (PlayerTankMainWidget)
		{
			PlayerTankMainWidget->AddToViewport();
		}
	}
}


void ATBasePlayerController::ResetPlayerTankMainWidget()
{
	if (!PlayerTankMainWidget)
	{
		CreatePlayerHUDWidget();
	}

	if (PlayerTankMainWidget && MyPlayerTank)
	{
		PlayerTankMainWidget->SetupForNewPlayerTankPawn(MyPlayerTank);
	}
}


void ATBasePlayerController::CreateScoreWidget()
{
	if (IsLocalPlayerController() && MultiplayerScoreBoardWidgetClass)
	{
		MultiplayerScoreBoard = CreateWidget<UTMultiplayerScoreBoard>(this, MultiplayerScoreBoardWidgetClass);
		if (MultiplayerScoreBoard)
		{
			MultiplayerScoreBoard->AddToViewport();
			MultiplayerScoreBoard->ShowScoreBoard(false);
		}
	}
}


void ATBasePlayerController::BindPlayerTankPawnEvents()
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && MyPlayerTank)
	{
		UTHealthComponent* PlayerTankHealthComp = MyPlayerTank->GetHealthComponent();
		if (PlayerTankHealthComp)
		{
			PlayerTankHealthComp->OnHealthChange.AddDynamic(this, &ATBasePlayerController::MyPlayerTankHealthChange);
		}
	}
}


/**************************************************************************/
/* Input */
/**************************************************************************/
void ATBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("ShowScoreBoard"), EInputEvent::IE_Pressed, this, &ATBasePlayerController::OnShowScoreBoard);
	InputComponent->BindAction(TEXT("ShowScoreBoard"), EInputEvent::IE_Released, this, &ATBasePlayerController::OnHideScoreBoard);
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


/**************************************************************************/
/* Damage (Health)  */
/**************************************************************************/
void ATBasePlayerController::MyPlayerTankHealthChange(float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* HealthChangeCauser)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		bool MyPlayerTankDied = FMath::IsNearlyZero(Health);
		UpdateGameModeForPlayerDamage(InstigatedBy, FMath::Abs(HealthDelta), MyPlayerTankDied);

		if (MyPlayerTankDied)
		{
			bEnablePawnControl = false;

			FString KillerPlayerName = GetKilledByPlayerName(InstigatedBy);
			ClientControlledPawnDied(KillerPlayerName);
			ClientControlledPawnDied_Implementation(KillerPlayerName);
		}
	}
}


void ATBasePlayerController::UpdateGameModeForPlayerDamage(AController* InstigatorController, float Damage, bool bMyPawnDied)
{
	// Update PlayerScore via GameMode
	AGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode();
	if (GameModeBase)
	{
		ATMultiplayerBaseGameMode* MultiplayerBaseGameMode = Cast<ATMultiplayerBaseGameMode>(GameModeBase);
		if (MultiplayerBaseGameMode)
		{
			MultiplayerBaseGameMode->AssignPlayerDamageScore(InstigatorController, this, Damage, bMyPawnDied);
		}
	}
}


FString ATBasePlayerController::GetKilledByPlayerName(AController* KillerController) const
{
	FString KillerPlayerName = FString();

	APlayerController* KillerPlayerController = Cast<APlayerController>(KillerController);
	if (KillerPlayerController)
	{
		APlayerState* KillerPlayerState = KillerPlayerController->PlayerState;
		if (KillerPlayerState)
		{
			KillerPlayerName = KillerPlayerState->GetPlayerName();
		}
	}

	return KillerPlayerName;
}


void ATBasePlayerController::ClientControlledPawnDied_Implementation(const FString& KillerPlayerName)
{
	if (PlayerTankMainWidget)
	{
		PlayerTankMainWidget->OwningPlayerTankKilled(KillerPlayerName);
	}
}


void ATBasePlayerController::SetControlledPawnCanTakeDamage(bool CanTakeDamage)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority && MyPlayerTank)
	{
		UTHealthComponent* PlayerTankHealthComp = MyPlayerTank->GetHealthComponent();
		if (PlayerTankHealthComp)
		{
			PlayerTankHealthComp->SetCanBeDamaged(CanTakeDamage);
		}
	}
}


void ATBasePlayerController::OnRep_DamageDisplay()
{
	SpawnDamageDisplayWidget(DamageDisplay.DamageCausedLocation, DamageDisplay.Damage);
}


void ATBasePlayerController::DisplayDamageCaused(AActor* DamagedActor, float Damage)
{
	if (DamagedActor)
	{
		DamageDisplay.Damage = Damage;
		DamageDisplay.DamageCausedLocation = DamagedActor->GetActorLocation();
		++DamageDisplay.DamageCausedCounter;

		if (GetLocalRole() == ENetRole::ROLE_Authority && IsLocalPlayerController())
		{
			OnRep_DamageDisplay();
		}
	}
}


/**************************************************************************/
/* State  */
/**************************************************************************/
void ATBasePlayerController::SetEnablePawnControl(bool Enable)
{
	if (MyPlayerTank)
	{
		if (Enable)
		{
			MyPlayerTank->EnableInput(this);
		}
		else
		{
			MyPlayerTank->DisableInput(this);
		}
	}
}


void ATBasePlayerController::OnRep_EnablePawnControl()
{
	/*APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	if (bEnablePawnControl)
	{
		MyPawn->EnableInput(this);
	}
	else
	{
		MyPawn->DisableInput(this);
	}*/
	SetEnablePawnControl(bEnablePawnControl);
}


void ATBasePlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	MyPlayerTank = Cast<ATPlayerTank>(P);
	BindPlayerTankPawnEvents();

	if (IsLocalPlayerController())
	{
		ResetPlayerTankMainWidget();
	}

	SetEnablePawnControl(bEnablePawnControl);
}


void ATBasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATBasePlayerController, DamageDisplay, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATBasePlayerController, bEnablePawnControl, COND_OwnerOnly);
}
