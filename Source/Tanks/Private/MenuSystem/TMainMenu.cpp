// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/TMainMenu.h"


// Engine Includes
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSubsystemUtils.h"


// Game Includes
#include "..\Public\MenuSystem\TServerRow.h"
#include "Subsystems/TSessionSubsystem.h"
#include "MenuSystem\TErrorAckWidget.h"


//bool UTMainMenu::Initialize()
//{
//	bool Success = Super::Initialize();
//	if (Success)
//	{
//		bIsFocusable = true;
//		Setup();
//		BindSessionSubsystemEvents();
//		Success = BindWidgetEvents();
//	}
//
//	return Success;
//}

bool UTMainMenu::BindSessionSubsystemEvents()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (GEngine && OnlineSub && OnlineSub->GetSubsystemName() == "NULL")
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, TEXT("Using NULL OnlineSubsystem"));
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return false;

	SessionSubsystem = GameInstance->GetSubsystem<UTSessionSubsystem>();
	if (!SessionSubsystem) return false;

	SessionSubsystem->OnCreateSessionCompleteEvent.AddDynamic(this, &UTMainMenu::OnCreateSessionComplete);
	SessionSubsystem->OnStartSessionCompleteEvent.AddDynamic(this, &UTMainMenu::OnStartSessionComplete);
	SessionSubsystem->OnDestroySessionCompleteEvent.AddDynamic(this, &UTMainMenu::OnDestroySessionComplete);
	SessionSubsystem->OnFindSessionsCompleteEvent.AddUObject(this, &ThisClass::OnFindSessionComplete);
	SessionSubsystem->OnJoinSessionCompleteEvent.AddUObject(this, &ThisClass::OnJoinSessionComplete);

	// Any time the main menu loads attempt to destroy the session. A session may or may not exist. Prevents the player
	// from having to restart the game to join and create a new session after they already joined or created one
	SessionSubsystem->DestroySession();

	return true;
}


bool UTMainMenu::BindWidgetEvents()
{
	if (!SinglePlayerButton) return false;
	SinglePlayerButton->OnClicked.AddDynamic(this, &UTMainMenu::OnSinglePlayerButtonClick);

	if (!MultiplayerButton) return false;
	MultiplayerButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerButtonClick);

	if (!SettingsButton) return false;
	SettingsButton->OnClicked.AddDynamic(this, &UTMainMenu::OnSettingsButtonClick);

	if (!SettingsBackButton) return false;
	SettingsBackButton->OnClicked.AddDynamic(this, &UTMainMenu::OnSettingsBackButtonClick);

	if (!ExitButton) return false;
	ExitButton->OnClicked.AddDynamic(this, &UTMainMenu::OnExitButtonClick);

	if (!CancelConfirmQuitButton) return false;
	CancelConfirmQuitButton->OnClicked.AddDynamic(this, &UTMainMenu::OnCancelConfirmQuitButtonClick);

	if (!QuitConfirmQuitButton) return false;
	QuitConfirmQuitButton->OnClicked.AddDynamic(this, &UTMainMenu::OnQuitConfirmQuitButtonClick);

	if (!MultiplayerHostButton) return false;
	MultiplayerHostButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerHostButtonClick);

	if (!MultiplayerJoinButton) return false;
	MultiplayerJoinButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerJoinButtonClick);

	if (!MultiplayerBackButton) return false;
	MultiplayerBackButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerBackButtonClick);

	if (!MultiplayerHostBackButton) return false;
	MultiplayerHostBackButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerHostBackButtonClick);

	if (!MultiplayerHostHostButton) return false;
	MultiplayerHostHostButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerHostHostButtonClick);

	if (!MultiplayerJoinBackButton) return false;
	MultiplayerJoinBackButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerJoinBackButtonClick);

	if (!MultiplayerJoinRefreshButton) return false;
	MultiplayerJoinRefreshButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerJoinRefreshButtonClick);

	if (!MultiplayerJoinJoinButton) return false;
	MultiplayerJoinJoinButton->OnClicked.AddDynamic(this, &UTMainMenu::OnMultiplayerJoinJoinButtonClick);

	if (!PlayerCountSlider) return false;
	PlayerCountSlider->OnValueChanged.AddDynamic(this, &UTMainMenu::OnPlayerCountSliderValueChange);

	return true;
}


/**************************************************************************/
/* Widget event bindings */
/**************************************************************************/
void UTMainMenu::OnSinglePlayerButtonClick()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Red, TEXT("MainMenuSinglePlayerButton Clicked: Not Implemented"));
	}
}


void UTMainMenu::OnMultiplayerButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, MultiplayerSubmenu);
}


void UTMainMenu::OnSettingsButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, SettingsSubmenu);
}


void UTMainMenu::OnSettingsBackButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, MainMenuSubmenu);
}


void UTMainMenu::OnExitButtonClick()
{
	SwitchSubmenu(QuitGameConfirmWidgetSwitcher, ConfirmQuitSubmenu);
}


void UTMainMenu::OnCancelConfirmQuitButtonClick()
{
	SwitchSubmenu(QuitGameConfirmWidgetSwitcher, MenuButtonsSubmenu);
}


void UTMainMenu::OnQuitConfirmQuitButtonClick()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
	}
}


void UTMainMenu::OnMultiplayerHostButtonClick()
{
	if (ServerNameTextBox)
	{
		ServerNameTextBox->SetText(FText());
	}

	SwitchSubmenu(MainMenuWidgetSwitcher, MultiplayerHostSubmenu);
}


void UTMainMenu::OnMultiplayerJoinButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, MultiplayerJoinSubmenu);
}


void UTMainMenu::OnMultiplayerBackButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, MainMenuSubmenu);
}


void UTMainMenu::OnMultiplayerHostBackButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, MainMenuSubmenu);
}


void UTMainMenu::OnMultiplayerHostHostButtonClick()
{
	CreateNewSession();
}


void UTMainMenu::OnMultiplayerJoinBackButtonClick()
{
	SwitchSubmenu(MainMenuWidgetSwitcher, MainMenuSubmenu);
}


void UTMainMenu::OnMultiplayerJoinRefreshButtonClick()
{
	SearchForSessionsToJoin();
}


void UTMainMenu::OnMultiplayerJoinJoinButtonClick()
{
	JoinSelectedServer();
}


void UTMainMenu::OnPlayerCountSliderValueChange(float Value)
{
	int32 SliderIntValue = FMath::FloorToInt(Value);
	if (PlayerCountText)
	{
		PlayerCountText->SetText(FText::FromString(FString::FromInt(SliderIntValue)));
	}
}


/**************************************************************************/
/* SessionSubsystem event callbacks */
/**************************************************************************/
void UTMainMenu::OnCreateSessionComplete(bool Successful)
{
	UE_LOG(LogTemp, Warning, TEXT("UTMainMenu::OnCreateSessionComplete, Success = %d"), Successful);
	
	if (Successful)
	{
		StartCreatedSession();
	}
	else
	{
		CreateErrorAckWidget(FString("Error Creating New Session"), FString());
	}
}


void UTMainMenu::OnStartSessionComplete(bool Successful)
{
	UE_LOG(LogTemp, Warning, TEXT("UTMainMenu::OnStartSessionComplete, Success = %d"), Successful);
	
	if (Successful)
	{
		HostOpenLevelAfterSessionStart();
	}
	else
	{
		CreateErrorAckWidget(FString("Error Starting New Session"), FString(""));
	}
}


void UTMainMenu::OnDestroySessionComplete(bool Successful)
{
	UE_LOG(LogTemp, Warning, TEXT("UTMainMenu::OnEndSessionComplete, Success = %d"), Successful);
}


void UTMainMenu::OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful)
{
	UE_LOG(LogTemp, Warning, TEXT("UTMainMenu::OnFindSessionComplete, Success = %d"), Successful);

	if (Successful)
	{
		FillServerListScrollBox(SessionResults);
	}
	else
	{
		CreateErrorAckWidget(FString("Error Finding Sessions"), FString(""));
	}
}


void UTMainMenu::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	bool Successful = Result == EOnJoinSessionCompleteResult::Success;

	UE_LOG(LogTemp, Warning, TEXT("UTMainMenu::OnJoinSessionComplete, Success = %d"), Successful);

	if (Successful)
	{
		ClientTravelToJoinedSession();
	}
	else
	{
		FString ErrorDetails;
		switch (Result)
		{
			case EOnJoinSessionCompleteResult::AlreadyInSession:
				ErrorDetails = "Already in selected session";
				break;
			case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
				ErrorDetails = "Unable to retrieve session address";
				break;
			case EOnJoinSessionCompleteResult::SessionDoesNotExist:
				ErrorDetails = "Selected session does not exist";
				break;
			case EOnJoinSessionCompleteResult::SessionIsFull:
				ErrorDetails = "Selected session is full. Select another session.";
				break;
			case EOnJoinSessionCompleteResult::UnknownError:
			default:
				ErrorDetails = "Unable to join selected session. Unknown error";
		}

		CreateErrorAckWidget(FString("Error Joining Session"), ErrorDetails);
	}
}


/**************************************************************************/
/* Host Session Helpers */
/**************************************************************************/
void UTMainMenu::CreateNewSession()
{
	if (!ServerNameTextBox || ! PlayerCountSlider) return;

	FText ServerName = ServerNameTextBox->GetText();

	if (ServerName.IsEmpty())
	{
		//
		//	Alert user server name can not be empty
		//
		CreateErrorAckWidget(FString("Unable To Create Server"), FString("Server name can not be empty!"));
		return;
	}

	// Get the amount of players to be in the session. There must be at least one player
	int32 PlayerCount = FMath::FloorToInt(PlayerCountSlider->GetValue());
	if (PlayerCount < 1)
	{
		PlayerCount = 1;
	}

	// Create the session using the session subsystem
	if (SessionSubsystem)
	{
		SessionSubsystem->CreateSession(PlayerCount, ServerName.ToString());
	}
}


void UTMainMenu::StartCreatedSession()
{
	if (SessionSubsystem)
	{
		SessionSubsystem->StartSession();
	}
}


void UTMainMenu::HostOpenLevelAfterSessionStart()
{
	TearDown();

	// TODO: Need to get map name to transition to
	UGameplayStatics::OpenLevel(GetWorld(), "/Game/Tanks/Map/MultiplayerLobby", true, "listen");
}


/**************************************************************************/
/* Join Session Helpers */
/**************************************************************************/
void UTMainMenu::UpdateJoinServerList()
{
	// When user selects a different server listed in ServerListScrollBox set that Server bSelected=true
	// and the rest to bSelected=false
	if (ServerListScrollBox)
	{
		for (int32 i = 0; i < ServerListScrollBox->GetChildrenCount(); i++)
		{
			UTServerRow* Row = Cast<UTServerRow>(ServerListScrollBox->GetChildAt(i));
			if (Row)
			{
				Row->bIsSelected = (SelectedServerRowIndex.IsSet() && SelectedServerRowIndex.GetValue() == i);
			}
		}
	}
}


void UTMainMenu::JoinSelectedServer()
{
	if (!SelectedServerRowIndex.IsSet())
	{
		CreateErrorAckWidget(FString("Select a Server To Join"), FString(""));
		return;
	}

	if (SessionSubsystem && ServerListScrollBox)
	{
		auto Widget = ServerListScrollBox->GetChildAt(SelectedServerRowIndex.GetValue());
		if (Widget)
		{
			auto ServerRowWidget = Cast<UTServerRow>(Widget);
			if (ServerRowWidget)
			{
				auto ServerRowSearchResults = ServerRowWidget->GetServerRowSearchResults();
				SessionSubsystem->JoinGameSession(ServerRowSearchResults);
			}
		}
	}
}


void UTMainMenu::SearchForSessionsToJoin()
{
	if (SessionSubsystem)
	{
		SessionSubsystem->FindSessions(100, true);
	}
}


void UTMainMenu::ClientTravelToJoinedSession()
{
	if (SessionSubsystem)
	{
		SessionSubsystem->TryTravelToCurrentSession();
	}
}


void UTMainMenu::FillServerListScrollBox(const TArray<FOnlineSessionSearchResult>& SessionResults)
{
	if (!ServerListScrollBox) return;

	ServerListScrollBox->ClearChildren();

	for (int32 i = 0; i < SessionResults.Num(); ++i)
	{
		if (SessionResults[i].IsValid())
		{
			auto ServerRow = CreateWidget<UTServerRow>(GetWorld(), ServerRowClass);
			if (ServerRow)
			{
				ServerRow->Setup(this, i, SessionResults[i]);
				ServerListScrollBox->AddChild(ServerRow);
			}
		}
	}
}


void UTMainMenu::SetSelectedServerRowIndex(uint32 Index)
{
	SelectedServerRowIndex = Index;
	UpdateJoinServerList();
}


void UTMainMenu::UpdateServerListScrollBox()
{
	if (!ServerListScrollBox) return;

	for (int32 i = 0; i < ServerListScrollBox->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UTServerRow>(ServerListScrollBox->GetChildAt(i));
		if (Row)
		{
			Row->bIsSelected = (SelectedServerRowIndex.IsSet() && SelectedServerRowIndex.GetValue() == i);
		}
	}
}

