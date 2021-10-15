// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TSessionSubsystem.h"

#include "OnlineSubsystemUtils.h"

UTSessionSubsystem::UTSessionSubsystem() :

	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionCompleted)),
	EndSessionCompleteDelegate(FOnEndSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnEndSessionCompleted)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleted)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleted))
{

}


void UTSessionSubsystem::CreateSession(int32 NumPublicConnections, FString ServerName)
{
	// Get Online Session Interface. If not valid alert OnCreateSessionCompleteEvent listeners CreateSession failed
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnCreateSessionCompleteEvent.Broadcast(false);
		return;
	}

	// Set settings for new session. As needed these settings should be set by the player via user interface.
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bShouldAdvertise = true;

	// If using the NULL subsystem then set IsLabMatch to true
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetSubsystemName() != "NULL")
	{
		LastSessionSettings->bIsLANMatch = false;
	}
	else
	{
		LastSessionSettings->bIsLANMatch = true;
	}

	// Set key value pair for server name
	LastSessionSettings->Set(SETTING_MAPNAME, ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// Bind delegate for async CreateSession function
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Attempt to create session using settings set in LastSessionSettings. CreateSession is async, see OnCreateSessionCompleted for success
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!LocalPlayer || !SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// If CreateSession fails immediately alert OnCreateSessionCompleteEvent listeners CreateSession failed
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		OnCreateSessionCompleteEvent.Broadcast(false);
	}
}


void UTSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool Successful)
{
	// Clear Delegate
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// Broadcast status of CreateSession success
	OnCreateSessionCompleteEvent.Broadcast(Successful);
}


void UTSessionSubsystem::StartSession()
{
	// Get Online Session Interface. If not valid alert OnStartSessionCompleteEvent listeners StartSession failed
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnStartSessionCompleteEvent.Broadcast(false);
		return;
	}

	// Bind delegate for async StartSession function
	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	// Attempt to start session. StartSession() is async, see OnStartSessionCompleted for success
	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		// If StartSession fails immediately alert OnStartSessionCompleteEvent listeners StartSession failed
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		OnStartSessionCompleteEvent.Broadcast(false);

	}
}


void UTSessionSubsystem::OnStartSessionCompleted(FName SessionName, bool Successful)
{
	// Clear Delegate
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}

	// Broadcast status of StartSession success
	OnStartSessionCompleteEvent.Broadcast(Successful);
}


void UTSessionSubsystem::EndSession()
{
	// Get Online Session Interface. If not valid alert OnEndSessionCompleteEvent listeners EndSession failed
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface)
	{
		OnEndSessionCompleteEvent.Broadcast(false);
		return;
	}

	// Bind delegate for async EndSession function
	EndSessionCompleteDelegateHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(EndSessionCompleteDelegate);

	// Attempt to end session. EndSession is async, see OnEndSessionCompleted for success
	if (!SessionInterface->EndSession(NAME_GameSession))
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteDelegateHandle);
		OnEndSessionCompleteEvent.Broadcast(false);
	}
}


void UTSessionSubsystem::OnEndSessionCompleted(FName SessionName, bool Successful)
{
	// Clear Delegate
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteDelegateHandle);
	}

	// Broadcast status of EndSession() success
	OnEndSessionCompleteEvent.Broadcast(Successful);
}


void UTSessionSubsystem::DestroySession()
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnDestroySessionCompleteEvent.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		OnDestroySessionCompleteEvent.Broadcast(false);
	}
}


void UTSessionSubsystem::OnDestroySessionCompleted(FName SessionName, bool Successful)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	OnDestroySessionCompleteEvent.Broadcast(Successful);
}


void UTSessionSubsystem::FindSessions(int32 MaxSearchResults, bool IsLanQuery)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface)
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub && OnlineSub->GetSubsystemName() != "NULL")
	{
		LastSessionSearch->bIsLanQuery = false;
	}
	else
	{
		LastSessionSearch->bIsLanQuery = true;
	}

	//LastSessionSearch->bIsLanQuery = IsLanQuery;

	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}


void UTSessionSubsystem::OnFindSessionsCompleted(bool Successful)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() > 0)
	{
		OnFindSessionsCompleteEvent.Broadcast(LastSessionSearch->SearchResults, Successful);
	}
	else
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), Successful);
	}
}


void UTSessionSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnJoinSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		OnJoinSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}


void UTSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	OnJoinSessionCompleteEvent.Broadcast(Result);
}


void UTSessionSubsystem::GetSessionSettings(FOnlineSessionSettings* OnlineSessionSettings)
{
	const IOnlineSessionPtr SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if (SessionInterface)
	{
		OnlineSessionSettings = SessionInterface->GetSessionSettings(GameSessionName);
	}
}

bool UTSessionSubsystem::TryTravelToCurrentSession()
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		return false;
	}

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString))
	{
		return false;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
	return true;
}
