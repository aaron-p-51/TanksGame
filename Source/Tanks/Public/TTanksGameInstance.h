// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "TMenuInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TTanksGameInstance.generated.h"


//class UUserWidget;
//class UMainMenu;
//class UTInGameMenu;
//class UButton;
//class FOnlineSessionSearch;
//class UTMultiplayerLobbyMenu;
//class UTErrorAckWidget;
//class FOnlineSessionSearchResult;
//class UTDialogWidget;



//USTRUCT()
//struct FHostSessionData
//{
//	GENERATED_BODY()
//
//public:
//	
//	TSharedPtr<const FUniqueNetId> UserID;
//
//	FName SessionName;
//
//	FString ServerName;
//	bool bIsLan;
//
//	bool bIsPresence;
//
//	int32 MaxPlayers;
//};

/**
 * 
 */
UCLASS()
class TANKS_API UTTanksGameInstance : public UGameInstance
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	//UPROPERTY()
	//FHostSessionData HostSessionData;

	///** Main Menu Widget Class */
	//UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	//TSubclassOf<UUserWidget> MainMenuWidgetClass;

	///** Main Menu */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	//UMainMenu* MainMenuWidget;

	///** In Game Menu Widget Class */
	//UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	//TSubclassOf<UUserWidget> InGameMenuWidgetClass;

	///** In game menu */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	//UTInGameMenu* InGameMenuWidget;

	///** Multiplayer Lobby Widget Class */
	//UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	//TSubclassOf<UUserWidget> MultiplayerLobbyWidgetClass;

	///** Multiplayer Lobby Widget */
	//UTMultiplayerLobbyMenu* MultiplayerLobbyWidget;


	//UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	//TSubclassOf<UUserWidget> DialogMessageWidgetClass;

	/*UTDialogWidget* DialogMessageWidget;*/


	//UPROPERTY(BlueprintReadOnly, Category = "Errors")
	//TArray<FString> Errors;


	//UPROPERTY(BlueprintReadWrite, Category = "Menu")
	//bool bSkipToMainMenuShowErrors = false;


	///** Host name of Server created */
	//FString UserHostServerName;

	///** Interface for Unreal's OnlineSubSystem */
	//IOnlineSessionPtr SessionInterface;

	///** Search results for found Servers */
	//TSharedPtr<FOnlineSessionSearch> SessionSearch;


	//TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	//UPROPERTY(BlueprintReadOnly, Category = "Multiplayer")
	//int32 HostingSessionMaxPlayers;

	//UPROPERTY(BlueprintReadOnly, Category = "Multiplayer")
	//FString HostingSessionServerName;

	//bool bStartNewSessionFlag = false;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer")
	//int32 MaxMultiplayerPlayerCount = 6;

	//int32 MultiplayerPlayerCount = 0;

	///** Delegate called when session created */
	//FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	///** Delegate called when session started */
	//FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	///** Handles to registered delegates for creating/starting a session */
	//FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	//FDelegateHandle OnStartSessionCompleteDelegateHandle;

	///** Delegate for searching for sessions */
	//FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	///** Handle to registered delegate for searching a session */
	//FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	///** Delegate for joining a session */
	//FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	///** Handle to registered delegate for joining a session */
	//FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	///** Delegate for destroying a session */
	//FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	///** Handle to registered delegate for destroying a session */
	//FDelegateHandle OnDestroySessionCompleteDelegateHandle;


/**
 * Methods
 */

 private:

	//void ShowSearchingForServerDialog();

protected:

	///**
	//*	Function to host a game!
	//*
	//*	@Param UserID		 - User that started the request
	//*	@Param SessionName	 - Name of the Session
	//*	@Param bIsLAN		 - Is this is LAN Game?
	//*	@Param bIsPresence	 - "Is the Session to create a presence Session"
	//*	@Param MaxNumPlayers - Number of Maximum allowed players on this "Session" (Server)
	//*/
	//bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString ServerName, bool bIsLan, bool bIsPresence, int32 MaxPlayers);
	
	///**
	//*	Function fired when a session create request has completed
	//*
	//* @param SessionName	- the name of the session this callback is for
	//* @param bWasSuccessful	- true if the async action completed without error, false if there was an error
	//*/
	//virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	///**
	//*	Function fired when a session start request has completed
	//*
	//*	@param SessionName		- the name of the session this callback is for
	//*	@param bWasSuccessful	- true if the async action completed without error, false if there was an error
	//*/
	//void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	///**
	//*	Find an online session
	//*
	//*	@param UserId		- user that initiated the request
	//*	@param bIsLAN		- are we searching LAN matches
	//*	@param bIsPresence	- are we searching presence sessions
	//*/
	//void FindSessions(TSharedPtr<const FUniqueNetId> UserID, bool bIsPresence);

	///**
	//*	Delegate fired when a session search query has completed
	//*
	//*	@param bWasSuccessful - true if the async action completed without error, false if there was an error
	//*/
	//void OnFindSessionsComplete(bool bWasSuccessful);

	///**
	//*	Joins a session via a search result
	//*
	//*	@param SessionName	- name of Session
	//*	@param SearchResult	- Session to join
	//*
	//*	@return bool true if successful, false otherwise
	//*/
	//bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	///**
	//*	Delegate fired when a session join request has completed
	//*
	//*	@param SessionName		- the name of the session this callback is for
	//*	@param bWasSuccessful	- true if the async action completed without error, false if there was an error
	//*/
	//void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	///**
	//*	Delegate fired when a destroying an online session has completed
	//*
	//*	@param SessionName		- the name of the session this callback is for
	//*	@param bWasSuccessful	- true if the async action completed without error, false if there was an error
	//*/
	//virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	//void CreateDialogWidget(FString DialogMessage);

	/*void RemoveDialogWidget();*/



public:

	UTTanksGameInstance(const FObjectInitializer& ObjectInitializer);

	/*virtual void Init() override;*/

	//UFUNCTION(BlueprintCallable, Category = "Network")
	//void HostOnlineGame(FString ServerName);

	//UFUNCTION(BlueprintCallable, Category = "Network")
	//void FindOnlineGames();

	//UFUNCTION(BlueprintCallable, Category = "Network")
	//void JoinOnlineGame();

	//UFUNCTION(BlueprintCallable, Category = "Network")
	//void DestroyAndLeaveGame();

	//UFUNCTION(BlueprintCallable, Category = "Menus")
	//void LoadMainMenuWidget();

	//UFUNCTION(BlueprintCallable, Category = "Menus")
	//void LoadMultiplayerMenuWidget();

	//UFUNCTION(BlueprintCallable, Category = "Menus")
	//void LoadInGameMenu();

	//UFUNCTION()
	//int32 GetHostingSessionMaxPlayers();

	//UFUNCTION()
	//FString GetHostingSessionServerName();
	
	//UTMultiplayerLobbyMenu* GetMultiplayerLobbyWidget();

	//UFUNCTION(BlueprintCallable)
	//void LoadMainMenuMap();

	//void NetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	

	
	



	//bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool )


};
