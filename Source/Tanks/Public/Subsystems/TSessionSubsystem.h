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

private:

	/** Bound to @CreateSessionCompleteDelegateHandle, CreateSession will call async functions wait for OnCreateSessionCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param SessionName			Name of created session
	 * @param Successful			Session was created successfully
	 */
	void OnCreateSessionCompleted(FName SessionName, bool Successful);

	/** Delegates for OnlineSession CreateSession async callbacks  */
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;


	/**************************************************************************/
	/* Start Session */
	/**************************************************************************/
public:

	/** Start the session created from @CreateSession  */ 
	void StartSession();

	/** Delegate for when StartSession has fully completed */
	FTOnStartSessionComplete OnStartSessionCompleteEvent;

private:

	/** Bound to @StartSessionCompleteDelegateHandle, StartSession will call async functions wait for OnStartSessionCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param SessionName			Name of created session
	 * @param Successful			Session was started successfully
	 */
	void OnStartSessionCompleted(FName SessionName, bool Successful);

	/** Delegates for OnlineSession StartSession async callbacks */
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;


	/**************************************************************************/
	/* End Session */
	/**************************************************************************/
public:

	/** End the current session */
	void EndSession();

	/** Delegate for when EndSession has fully completed */
	FTOnEndSessionComplete OnEndSessionCompleteEvent;

private:

	/** Bound to @EndSessionCompleteDelegateHandle, EndSession will call async functions wait for OnEndSessionCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param SessionName			Name of created session
	 * @param Successful			Session was ended successfully
	 */
	void OnEndSessionCompleted(FName SessionName, bool Successful);


	/** Delegates for OnlineSession EndSession async callbacks */
	FOnEndSessionCompleteDelegate EndSessionCompleteDelegate;
	FDelegateHandle EndSessionCompleteDelegateHandle;


	/**************************************************************************/
	/* Destroy Session */
	/**************************************************************************/
public:

	/** Destroy the current session */
	void DestroySession();

	/** Delegate for when DestroySession has fully completed */
	FTOnDestroySessionComplete OnDestroySessionCompleteEvent;

private:

	/** Bound to @DestroySessionCompleteDelegateHandle, DestroySession will call async functions wait for OnDestroySessionCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param SessionName			Name of created session
	 * @param Successful			Session was destroyed successfully
	 */
	void OnDestroySessionCompleted(FName SessionName, bool Successful);

	/** Delegates for OnlineSession DestroySession async callbacks */
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;


	/**************************************************************************/
	/* Find Session */
	/**************************************************************************/
public:

	/** Find a session to join
	 *
	 * @param MaxSearchResults		When OnFindSessionsCompleteEvent is broadcasted no more than this many session will be sent
	*/
	void FindSessions(int32 MaxSearchResults);

	/** Delegate for when FindSession has fully completed */
	FTOnFindSessionComplete OnFindSessionsCompleteEvent;


private:

	/** Bound to @FindSessionsCompleteDelegateHandle, JoinSession will call async functions wait for OnFindSessionsCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param Successful			Session was started successfully
	 */
	void OnFindSessionsCompleted(bool Successful);


	/** Delegates for OnlineSession FindSession  async callbacks */
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;


	/**************************************************************************/
	/* Join Session */
	/**************************************************************************/
public:

	/** Join session passed in SessionResult */
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);

	/** Delegate for when JoinGameSession has fully completed */
	FTOnJoinSessionComplete OnJoinSessionCompleteEvent;

	/** Try to travel to the currently joined session */
	bool TryTravelToCurrentSession();

private:

	/** Bound to @JoinSessionCompleteDelegateHandle, JoinSession will call async functions wait for OnJoinSessionCompleted to be
	 * called before proceeding with additional session calls
	 *
	 * @param SessionName			Name of Session attempted to join
	 * @param Result				Result of attempting to join session
	 */
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	/** Delegates for OnlineSession JoinSession async callbacks */
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;


	/**************************************************************************/
	/* Accessors  */
	/**************************************************************************/
	void GetSessionSettings(FOnlineSessionSettings* OnlineSessionSettings);

};
