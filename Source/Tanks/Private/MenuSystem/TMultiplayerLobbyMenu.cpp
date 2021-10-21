// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MenuSystem/TMultiplayerLobbyMenu.h"


// Engine Includes
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


// Game Includes
#include "Player/TLobbyPlayerController.h"
#include "Player/TLobbyPlayerState.h"
#include "GameStates/TLobbyGameState.h"
#include "MenuSystem/TPlayerInLobby.h"
#include "Subsystems/TSessionSubsystem.h"

bool UTMultiplayerLobbyMenu::Initialize()
{
	bool Success = Super::Initialize();

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(TimerHandler_UpdateLobbyData, this, &UTMultiplayerLobbyMenu::UpdateLobbyData, UpdateLobbyDataRate, true);
	}

	InitWidgetProperties();


	return Success;
}


void UTMultiplayerLobbyMenu::InitWidgetProperties()
{
	if (VoteToStartBox)
	{
		VoteToStartBox->SetVisibility(ESlateVisibility::Hidden);
	}

	if (MatchStartText)
	{
		MatchStartText->SetText(FText::FromString(TEXT("Waiting for players")));
	}

	if (ServerNameText)
	{
		ServerNameText->SetVisibility(ESlateVisibility::Hidden);
	}
}


bool UTMultiplayerLobbyMenu::BindSessionSubsystemEvents()
{
	if (SessionSubsystem)
	{
		SessionSubsystem->OnEndSessionCompleteEvent.AddDynamic(this, &UTMultiplayerLobbyMenu::OnEndSessionComplete);
		return true;
	}

	return false;
}


bool UTMultiplayerLobbyMenu::BindWidgetEvents()
{
	if (!VoteToStartCheckBox) return false;
	VoteToStartCheckBox->OnCheckStateChanged.AddDynamic(this, &UTMultiplayerLobbyMenu::OnVoteToStartCheckBoxChange);

	if (!QuitToMainMenuButton) return false;
	QuitToMainMenuButton->OnClicked.AddDynamic(this, &UTMultiplayerLobbyMenu::OnQuitToMainMenuButtonClick);

	if (!ConfirmQuitButton) return false;
	ConfirmQuitButton->OnClicked.AddDynamic(this, &UTMultiplayerLobbyMenu::OnConfirmQuitButtonClick);

	if (!CancelQuitButton) return false;
	CancelQuitButton->OnClicked.AddDynamic(this, &UTMultiplayerLobbyMenu::OnCancelQuitButtonClick);

	UE_LOG(LogTemp, Warning, TEXT("LobbyWidget, BindWidgetEvents()"));

	return true;
}


void UTMultiplayerLobbyMenu::UpdateLobbyData()
{
	if (!LobbyGameState)
	{
		LobbyGameState = GetLobbyGameState();
		return;
	}

	FLobbyData LobbyData = LobbyGameState->GetLobbyData();

	UpdateServerName(LobbyData.ServerName);
	UpdateVotesToStartText(LobbyData.VotesToStart, LobbyData.VotesNeededToStart);
	UpdatePlayersInLobbyCount(LobbyData.CurrentPlayers, LobbyData.MaxPlayers);
	UpdatePlayersInLobby(LobbyGameState);

	if (LobbyData.bIsMatchStarting)
	{
		UpdateTimeTillMatchStart(FMath::FloorToInt(LobbyData.TimeTillMatchStart));
		if (LobbyData.TimeTillMatchStart <= 0.f)
		{
			TearDown();
		}
	}
}


ATLobbyGameState* UTMultiplayerLobbyMenu::GetLobbyGameState() const
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (!GameState) return nullptr;

	return Cast<ATLobbyGameState>(GameState);
}


void UTMultiplayerLobbyMenu::UpdateVotesToStartText(int32 Votes, int32 VotesNeeded)
{
	if (VoteToStartBox)
	{
		VoteToStartBox->SetVisibility(ESlateVisibility::Visible);
	}

	if (CurrentVotesToStartText)
	{
		CurrentVotesToStartText->SetText(FText::FromString(FString::FromInt(Votes)));
	}

	if (NeededVotesToStartText)
	{
		NeededVotesToStartText->SetText(FText::FromString(FString::FromInt(VotesNeeded)));
	}
}


void UTMultiplayerLobbyMenu::UpdateServerName(const FString& ServerName)
{
	if (!ServerNameText) return;

	ServerNameText->SetVisibility(ESlateVisibility::Visible);

	FString ServerNameUpper = ServerName.ToUpper();

	// check to see if the server/session name contains the word server. If not then add the string "server" to the end
	if (ServerNameUpper.Contains(FString("SERVER")))
	{
		ServerNameText->SetText(FText::FromString(ServerName));
	}
	else
	{
		ServerNameText->SetText(FText::FromString(ServerName + " Server"));
	}
}


void UTMultiplayerLobbyMenu::UpdateTimeTillMatchStart(int32 SecondsTillMatchStart)
{
	if (NextMapBox)
	{
		NextMapBox->SetVisibility(ESlateVisibility::Visible);
	}

	if (MatchStartText)
	{
		MatchStartText->SetVisibility(ESlateVisibility::Visible);
		MatchStartText->SetText(FText::FromString(TEXT("Next map starting in")));
	}

	if (MatchStartTimeText)
	{
		MatchStartTimeText->SetVisibility(ESlateVisibility::Visible);

		FText SecondsTimeText = FText::FromString(FString::FromInt(0));
		if (SecondsTillMatchStart > 0)
		{
			SecondsTimeText = FText::FromString(FString::FromInt(SecondsTillMatchStart));
		}

		MatchStartTimeText->SetText(SecondsTimeText);
	}
}


void UTMultiplayerLobbyMenu::UpdatePlayersInLobby(ATLobbyGameState* GameState)
{
	if (!PlayersBox || !GameState) return;

	for (auto Player : LobbyGameState->PlayerArray)
	{
		if (!PlayerInLobbyMap.Find(Player))
		{
			UTPlayerInLobby* PlayerInLobbyWidget = CreateWidget<UTPlayerInLobby>(GetWorld(), PlayerInLobbyWidgetClass);
			if (PlayerInLobbyWidget)
			{
				PlayerInLobbyWidget->SetPlayerName(FText::FromString(Player->GetPlayerName()));
				PlayerInLobbyMap.Add(Player, PlayerInLobbyWidget);
				PlayersBox->AddChildToVerticalBox(PlayerInLobbyWidget);
			}
		}
	}

	////////////////////////////////////////////////////////
	////
	//// Debug
	////
	/////////////////////////////////////////////////////////

	//TArray<UUserWidget*> FoundWidgets;

	//UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, PlayerInLobbyWidgetClass, false);
	//
	//int32 FoundWidgetCount = 0;
	//for (auto& Widget : FoundWidgets)
	//{
	//	if (Cast<UTPlayerInLobby>(Widget) != nullptr)
	//	{
	//		++FoundWidgetCount;
	//	}
	//}

	//UE_LOG(LogTemp, Warning, TEXT("Found %d widgets of type PlayerInLobbyWidgetClass"), FoundWidgetCount);
}


void UTMultiplayerLobbyMenu::UpdatePlayersInLobbyCount(const int32 CurrentPlayers, const int32 MaxPlayers)
{
	if (PlayerCountText)
	{
		PlayerCountText->SetText(FText::FromString(FString::FromInt(CurrentPlayers)));
	}

	if (MaxPlayersText)
	{
		MaxPlayersText->SetText(FText::FromString(FString::FromInt(MaxPlayers)));
	}
}


/**************************************************************************/
/* Widget event bindings */
/**************************************************************************/
void UTMultiplayerLobbyMenu::OnVoteToStartCheckBoxChange(bool IsChecked)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	ATLobbyPlayerController* LobbyPlayerController = Cast<ATLobbyPlayerController>(PlayerController);
	if (!LobbyPlayerController) return;

	LobbyPlayerController->VoteToStart(IsChecked);
}


void UTMultiplayerLobbyMenu::OnQuitToMainMenuButtonClick()
{
	bool Success = SwitchSubmenu(QuitWidgetSwitcher, ConfirmQuitSubmenu);
	if (Success && ConfirmQuitText)
	{
		// Check to see if the local player is hosting the session. If so alert the player all client will be disconnected upon quitting
		APlayerController* OwningPlayerController = GetOwningLocalPlayer()->GetPlayerController(GetWorld());
		if (OwningPlayerController && OwningPlayerController->HasAuthority())
		{
			ConfirmQuitText->SetText(FText::FromString("Warning you are the Host.All clients will be disconnected"));
		}
		else
		{
			ConfirmQuitText->SetText(FText::FromString("Are you sure"));
		}
	}
}


void UTMultiplayerLobbyMenu::OnConfirmQuitButtonClick()
{
	if (SessionSubsystem)
	{
		SessionSubsystem->EndSession();
	}
}


void UTMultiplayerLobbyMenu::OnCancelQuitButtonClick()
{
	SwitchSubmenu(QuitWidgetSwitcher,  QuitSubmenu);
}


void UTMultiplayerLobbyMenu::OnEndSessionComplete(bool Successful)
{
	UE_LOG(LogTemp, Warning, TEXT("UMInGameMenu::OnEndSessionComplete, Success = %d"), Successful);
	UGameplayStatics::OpenLevel(GetWorld(), "/Game/Tanks/Map/MainMenu", true);
}
