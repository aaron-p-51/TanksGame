// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Interface include for OnlineSessions
#include "Interfaces/OnlineSessionInterface.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "TSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTOnCreateSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTOnStartSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTOnEndSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTOnDestroySessionComplete, bool, Successful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FTOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>&, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FTOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type);


/**
 * 
 */
UCLASS()
class TANKS_API UTSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UTSessionSubsystem();

	/**************************************************************************/
	/* Create Session */
	/**************************************************************************/
public:

	/**
	 * Creates a new session. New session settings will be set in a TSharedPtr<FOnlineSessionSettings>
	 * as needed change argument list to match what settings need to be set on a per session basis. ie
	 * not the samge for every game. @param ServerName should always be included
	 *
	 * @param NumPublicConnections		Number of public connection this new session will allow
	 * @param ServerName				Name for this new session
	 */
	void CreateSession(int32 NumPublicConnections, FString ServerName);

	/** Delegate for when when CreateSession has fully completed */
	FTOnCreateSessionComplete OnCreateSessionCompleteEvent;

protected:

	/** Bound to @OnCreateSessionCompleteEvent, CreateSession will call async functions wait for OnCreateSessionCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param SessionName			Name of created session
	 * @param Successful			true or session was created successfully
	 */
	void OnCreateSessionCompleted(FName SessionName, bool Successful);


private:

	// Create Session
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	/**************************************************************************/
	/* Start Session */
	/**************************************************************************/
public:

	// Start Session
	void StartSession();
	FTOnStartSessionComplete OnStartSessionCompleteEvent;

protected:

	// Start Session
	void OnStartSessionCompleted(FName SessionName, bool Successful);

private:

	// Start Session
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	/**************************************************************************/
	/* End Session */
	/**************************************************************************/
public:

	// End Session
	void EndSession();
	FTOnEndSessionComplete OnEndSessionCompleteEvent;

protected:

	// End Session
	void OnEndSessionCompleted(FName SessionName, bool Successful);

private:

	// End Session
	FOnEndSessionCompleteDelegate EndSessionCompleteDelegate;
	FDelegateHandle EndSessionCompleteDelegateHandle;

	/**************************************************************************/
	/* Destroy Session */
	/**************************************************************************/
public:

	// Destroy Session
	void DestroySession();
	FTOnDestroySessionComplete OnDestroySessionCompleteEvent;

protected:


	// Destroy Session
	void OnDestroySessionCompleted(FName SessionName, bool Successful);

private:

	// Destroy Session
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	/**************************************************************************/
	/* Find Session */
	/**************************************************************************/
public:
	// Find Sessions
	void FindSessions(int32 MaxSearchResults, bool IsLanQuery);
	FTOnFindSessionComplete OnFindSessionsCompleteEvent;


protected:

	// Find Sessions
	void OnFindSessionsCompleted(bool Successful);

private:

	// Find Sessions
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	/**************************************************************************/
	/* Join Session */
	/**************************************************************************/
public:

	// Join Sessions
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);
	FTOnJoinSessionComplete OnJoinSessionCompleteEvent;

	bool TryTravelToCurrentSession();

protected:

	// Join Session
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:

	// Join Session
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;


	/**************************************************************************/
	/* Accessors  */
	/**************************************************************************/
	void GetSessionSettings(FOnlineSessionSettings* OnlineSessionSettings);

};
