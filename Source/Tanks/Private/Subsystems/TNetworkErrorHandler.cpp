// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TNetworkErrorHandler.h"

void UTNetworkErrorHandler::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Make sure there is a game instance and Eninge, prevent crashing in editor
	UGameInstance* GM = GetGameInstance();
	if (!GM) return;
	UEngine* Engine = GM->GetEngine();
	if (!Engine) return;

	Engine->OnNetworkFailure().AddUObject(this, &UTNetworkErrorHandler::HandleNetworkFailure);
}

void UTNetworkErrorHandler::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{

}
