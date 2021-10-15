// Fill out your copyright notice in the Description page of Project Settings.


#include "TTanksGameInstance.h"

// Engine Includes
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

// Game Includes


const static FName SESSION_NAME = TEXT("GameSession");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");



UTTanksGameInstance::UTTanksGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UTTanksGameInstance::OnCreateSessionComplete);
	//OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UTTanksGameInstance::OnStartOnlineGameComplete);
	//OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UTTanksGameInstance::OnFindSessionsComplete);
	//OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UTTanksGameInstance::OnJoinSessionComplete);
	//OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTTanksGameInstance::OnDestroySessionComplete);
}


//void UTTanksGameInstance::Init()
//{
//
//
//
//
//	Super::Init();
//
//	//UEngine* Engine = GetEngine();
//	//if (Engine)
//	//{
//	//	Engine->OnNetworkFailure().AddUObject(this, &UTTanksGameInstance::NetworkError);
//	//}
//
//}


//void UTTanksGameInstance::HostOnlineGame(FString ServerName)
//{
//
//
//	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
//
//	HostSessionData.UserID = netID;
//	HostSessionData.ServerName = ServerName;
//	HostSessionData.SessionName = GameSessionName;
//	HostSessionData.bIsLan = true;
//	HostSessionData.bIsPresence = true;
//	HostSessionData.MaxPlayers = 4;
//	bStartNewSessionFlag = true;
//
//	DestroyAndLeaveGame();
//
//	HostSession(netID, GameSessionName, ServerName, true, true, 4);
//
//}


//void UTTanksGameInstance::FindOnlineGames()
//{
//	const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
//
//	FindSessions(netID, true);
//}


//void UTTanksGameInstance::JoinOnlineGame()
//{
//	if (MainMenuWidget)
//	{
//		TOptional<uint32> SessionSearchResultIndex = MainMenuWidget->GetSelectedServerIndex();
//		if (SessionSearchResultIndex.IsSet())
//		{
//			int32 SessionIndex = SessionSearchResultIndex.GetValue();
//			if (SessionIndex < 0 || SessionIndex > SessionSearch->SearchResults.Num())
//			{
//				if (GEngine)
//				{
//					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("JoinOnlineGame Error, Invalid SessionSearchServerIndex: %d"), SessionIndex));
//				}
//			}
//
//			const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
//			ULocalPlayer* const Player = GetFirstGamePlayer();
//
//			if (SessionSearch->SearchResults[SessionIndex].Session.OwningUserId == Player->GetPreferredUniqueNetId())
//			{
//				if (GEngine)
//				{
//					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("JoinOnlineGame Error, Can not join your own Session!!!")));
//				}
//			}
//
//			JoinSession(netID, GameSessionName, SessionSearch->SearchResults[SessionIndex]);
//		}
//	}
//
//	//const TSharedPtr<const FUniqueNetId> netID = UGameplayStatics::GetGameInstance(GetWorld())->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
//
//	//ULocalPlayer* const Player = GetFirstGamePlayer();
//
//	//// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
//	//FOnlineSessionSearchResult SearchResult;
//
//	//// If the Array is not empty, we can go through it
//	//if (SessionSearch->SearchResults.Num() > 0)
//	//{
//	//	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
//	//	{
//	//		// To avoid something crazy, we filter sessions from ourself
//	//		if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
//	//		{
//	//			SearchResult = SessionSearch->SearchResults[i];
//
//	//			// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
//	//			// use a widget where you click on and have a reference for the GameSession it represents which you can use
//	//			// here
//	//			JoinSession(netID, GameSessionName, SearchResult);
//	//			break;
//	//		}
//	//	}
//	//}
//}


//void UTTanksGameInstance::DestroyAndLeaveGame()
//{
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//
//
//		if (Sessions.IsValid())
//		{
//			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
//
//			UWorld* World = GetWorld();
//			if (World)
//			{
//				auto PC = World->GetFirstPlayerController();
//				if (PC)
//				{
//					PC->DisableInput(PC);
//				}
//			}
//
//			Sessions->DestroySession(GameSessionName);
//
//			return;
//		}
//	}
//
//	if (bStartNewSessionFlag)
//	{
//		bStartNewSessionFlag = false;
//		HostSession(HostSessionData.UserID, HostSessionData.SessionName, HostSessionData.ServerName, HostSessionData.bIsLan, HostSessionData.bIsPresence, HostSessionData.MaxPlayers);
//	}
//}




//bool UTTanksGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString ServerName, bool bIsLan, bool bIsPresence, int32 MaxPlayers)
//{
//	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
//
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid() && UserId.IsValid())
//		{
//			SessionSettings = MakeShareable(new FOnlineSessionSettings());
//
//			if (OnlineSub->GetSubsystemName() == "NULL")
//			{
//				SessionSettings->bIsLANMatch = true;
//			}
//			else
//			{
//				SessionSettings->bIsLANMatch = false;
//			}
//
//			if (GEngine)
//			{
//				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("IsLanMatch: %d"), SessionSettings->bIsLANMatch));
//			}
//
//			SessionSettings->bUsesPresence = bIsPresence;
//			SessionSettings->NumPublicConnections = MaxPlayers;
//			SessionSettings->NumPrivateConnections = 0;
//			SessionSettings->bAllowInvites = true;
//			SessionSettings->bAllowJoinInProgress = true;
//			SessionSettings->bShouldAdvertise = true;
//			SessionSettings->bAllowJoinViaPresence = true;
//			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
//
//			SessionSettings->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);
//			SessionSettings->Set(SERVER_NAME_SETTINGS_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
//
//			// Set the delegate to the Handle of the SessionInterface
//			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
//
//
//			// Delegate will get called when complete, this return value does not indicate session was successfully created
//			//CreateDialogWidget(TEXT("Creating Server"));
//			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
//		}
//	}
//	else
//	{
//		if (GEngine)
//		{
//			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No Onlinesubsystme found!"));
//		}
//	}
//
//	return false;
//}


//void UTTanksGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
//{
//	RemoveDialogWidget();
//
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
//	}
//
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid())
//		{
//			// Clear SessionComplete delegate handle that was set in HostSession()
//			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
//			if (bWasSuccessful)
//			{
//				// Set the StartSession delegate handle
//				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
//
//
//				//CreateDialogWidget(TEXT("Start Game Session"));
//
//				FOnlineSessionSettings* CreatedSessionSettings = Sessions->GetSessionSettings(SessionName);
//				if (CreatedSessionSettings)
//				{
//					HostingSessionMaxPlayers = CreatedSessionSettings->NumPublicConnections;
//					CreatedSessionSettings->Get(SERVER_NAME_SETTINGS_KEY, HostingSessionServerName);
//				}
//
//
//				// StartSessionComplete delegate should get called when StartSession is complete
//				Sessions->StartSession(SessionName);
//			}
//		}
//	}
//}

//void UTTanksGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
//{
//	RemoveDialogWidget();
//
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
//	}
//
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid())
//		{
//			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
//		}
//	}
//
//	if (bWasSuccessful)
//	{
//		UWorld* World = GetWorld();
//		if (World)
//		{
//			UGameViewportClient* CurrentViewPort = World->GetGameViewport();
//			if (CurrentViewPort)
//			{
//				CurrentViewPort->RemoveAllViewportWidgets();
//			}
//
//			UGameplayStatics::OpenLevel(GetWorld(), "/Game/Tanks/Maps/MultiplayerLobby", true, "listen");
//		}
//	}
//}

//void UTTanksGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserID, bool bIsPresence)
//{
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//
//		if (Sessions.IsValid() && UserID.IsValid())
//		{
//			SessionSearch = MakeShareable(new FOnlineSessionSearch());
//
//			if (OnlineSub->GetSubsystemName() == "NULL")
//			{
//				SessionSearch->bIsLanQuery = true;
//			}
//			else
//			{
//				SessionSearch->bIsLanQuery = false;
//			}
//
//			SessionSearch->MaxSearchResults = 20;
//			SessionSearch->PingBucketSize = 50;
//
//			if (bIsPresence)
//			{
//				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
//			}
//
//			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
//			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
//
//			ShowSearchingForServerDialog();
//
//
//			Sessions->FindSessions(*UserID, SearchSettingsRef);
//		}
//	}
//	else
//	{
//		OnFindSessionsComplete(false);
//	}
//}

//void UTTanksGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
//{
//	RemoveDialogWidget();
//
//	// Debug
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful));
//	}
//
//	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid())
//		{
//			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
//
//			// Debug
//			if (GEngine)
//			{
//				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));
//			}
//
//			if (SessionSearch->SearchResults.Num() > 0)
//			{
//
//				TArray<FServerSettings> Servers;
//				for (int32 SearchIndex = 0; SearchIndex < SessionSearch->SearchResults.Num(); SearchIndex++)
//				{
//					// Debug
//					if (GEngine)
//					{
//						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIndex + 1, *(SessionSearch->SearchResults[SearchIndex].Session.OwningUserName)));
//					}
//
//					FServerSettings SearchResultServerSettings;
//
//					FString SearchResultsServerName;
//					if (SessionSearch->SearchResults[SearchIndex].Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, SearchResultsServerName))
//					{
//						SearchResultServerSettings.Name = SearchResultsServerName;
//					}
//					else
//					{
//						SearchResultServerSettings.Name = TEXT("Unknow Server Name");
//					}
//
//					SearchResultServerSettings.HostUserName = SessionSearch->SearchResults[SearchIndex].Session.OwningUserName;
//					SearchResultServerSettings.MaxPlayers = SessionSearch->SearchResults[SearchIndex].Session.SessionSettings.NumPublicConnections;
//					SearchResultServerSettings.CurrentPlayers = SearchResultServerSettings.MaxPlayers - SessionSearch->SearchResults[SearchIndex].Session.NumOpenPublicConnections;
//
//					Servers.Add(SearchResultServerSettings);
//				}
//
//				if (MainMenuWidget)
//				{
//					//MainMenuWidget->SetServerList(Servers);
//				}
//			}
//		}
//	}
//}


//bool UTTanksGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
//{
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, (TEXT("JoinSession")));
//	}
//	bool bSuccessful = false;
//
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//
//		if (Sessions.IsValid() && UserId.IsValid())
//		{
//			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
//
//			FString SessionNameString = FString::Printf(TEXT("Joining %s"), *SessionName.ToString());
//			//CreateDialogWidget(SessionNameString);
//			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
//		}
//	}
//
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("JoinSession, bSuccess: %d"), bSuccessful));
//	}
//	return bSuccessful;
//}


//void UTTanksGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
//{
//	RemoveDialogWidget();
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));
//	}
//
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid())
//		{
//			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
//
//			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
//			// This is something the Blueprint Node "Join Session" does automatically!
//			APlayerController* const PC = GetFirstLocalPlayerController();
//
//			FString TravelURL;
//			if (PC && Sessions->GetResolvedConnectString(SessionName, TravelURL))
//			{
//				UWorld* World = GetWorld();
//				if (World)
//				{
//					UGameViewportClient* CurrentViewPort = World->GetGameViewport();
//					if (CurrentViewPort)
//					{
//						CurrentViewPort->RemoveAllViewportWidgets();
//					}
//				}
//				PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
//			}
//		}
//	}
//}


//void UTTanksGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
//{
//	UWorld* World = GetWorld();
//	if (World)
//	{
//		auto PC = World->GetFirstPlayerController();
//		if (PC)
//		{
//			PC->EnableInput(PC);
//		}
//	}
//
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
//	}
//
//	/*HostingSessionMaxPlayers = 0;
//	HostingSessionServerName = "";*/
//
//	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//
//		if (Sessions.IsValid())
//		{
//			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
//
//			if (bWasSuccessful)
//			{
//				//UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true);
//			}
//		}
//	}
//
//	if (bStartNewSessionFlag)
//	{
//		bStartNewSessionFlag = false;
//		HostSession(HostSessionData.UserID, HostSessionData.SessionName, HostSessionData.ServerName, HostSessionData.bIsLan, HostSessionData.bIsPresence, HostSessionData.MaxPlayers);
//	}
//}


//void UTTanksGameInstance::CreateDialogWidget(FString DialogMessage, EDialogType DialogType)
//{
//	// Check if existing dialog message, if it does append message
//	if (DialogMessageWidget)
//	{
//		DialogMessageWidget->AppendToExistingDialogMessage(DialogMessage);
//	}
//	else
//	{
//		if (DialogMessageWidgetClass)
//		{
//			DialogMessageWidget = CreateWidget<UTDialogWidget>(this, DialogMessageWidgetClass);
//			if (DialogMessageWidget)
//			{
//				DialogMessageWidget->SetDialogMessage(DialogMessage, DialogType);
//				DialogMessageWidget->Setup();
//			}
//		}
//	}
//}

//void UTTanksGameInstance::ShowSearchingForServerDialog()
//{
//	if (DialogMessageWidget)
//	{
//		DialogMessageWidget->TearDown(ETearDownOption::ETDO_Remove);
//	}
//
//	if (DialogMessageWidgetClass)
//	{
//		DialogMessageWidget = CreateWidget<UTDialogWidget>(this, DialogMessageWidgetClass);
//		if (DialogMessageWidget)
//		{
//			DialogMessageWidget->SetDialogMessage(TEXT("Searching For Servers"), EDialogType::EDT_SearchingForServers);
//			DialogMessageWidget->Setup();
//		}
//	}
//}

//void UTTanksGameInstance::RemoveDialogWidget()
//{
//	if (DialogMessageWidget)
//	{
//		DialogMessageWidget->RemoveFromViewport();
//		//DialogMessageWidget->Destruct();
//		DialogMessageWidget = nullptr;
//	}
//}


//void UTTanksGameInstance::LoadMainMenuWidget()
//{
//	if (MainMenuWidgetClass)
//	{
//		MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenuWidgetClass);
//		if (MainMenuWidget)
//		{
//			//MainMenuWidget->Setup();
//		}
//	}
//}

//void UTTanksGameInstance::LoadMultiplayerMenuWidget()
//{
//	if (MultiplayerLobbyWidgetClass)
//	{
//		MultiplayerLobbyWidget = CreateWidget<UTMultiplayerLobbyMenu>(this, MultiplayerLobbyWidgetClass);
//		if (MultiplayerLobbyWidget)
//		{
//			MultiplayerLobbyWidget->Setup();
//		}
//	}
//}

//void UTTanksGameInstance::LoadInGameMenu()
//{
//	if (InGameMenuWidgetClass)
//	{
//		InGameMenuWidget = CreateWidget<UTInGameMenu>(this, InGameMenuWidgetClass);
//		if (InGameMenuWidget)
//		{
//			InGameMenuWidget->Setup();
//		}
//	}
//}

//int32 UTTanksGameInstance::GetHostingSessionMaxPlayers()
//{
//	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
//
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid())
//		{
//			auto CurrentSessionSettings = Sessions->GetSessionSettings(SESSION_NAME);
//			if (CurrentSessionSettings)
//			{
//				return CurrentSessionSettings->NumPublicConnections;
//			}
//		}
//	}
//	
//	return 0;
//}

//FString UTTanksGameInstance::GetHostingSessionServerName()
//{
//	FString CurrentServerName;
//
//	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
//
//	if (OnlineSub)
//	{
//		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
//		if (Sessions.IsValid())
//		{
//			auto CurrentSessionSettings = Sessions->GetSessionSettings(SESSION_NAME);
//			if (CurrentSessionSettings)
//			{
//				CurrentSessionSettings->Get(SERVER_NAME_SETTINGS_KEY, CurrentServerName);	
//			}
//		}
//	}
//
//	return CurrentServerName;
//}

//UTMultiplayerLobbyMenu* UTTanksGameInstance::GetMultiplayerLobbyWidget()
//{
//	return MultiplayerLobbyWidget;
//}

//void UTTanksGameInstance::LoadMainMenuMap()
//{
//	APlayerController* PlayerController = GetFirstLocalPlayerController();
//	if (PlayerController == nullptr) return;
//
//	PlayerController->ClientTravel("/Game/Tanks/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
//
//}

//void UTTanksGameInstance::NetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
//{
//	if (GEngine)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Network Error Captured: %s"), *ErrorString));
//	}
//
//	LoadMainMenuMap();
//}




