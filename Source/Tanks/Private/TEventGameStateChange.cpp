// Fill out your copyright notice in the Description page of Project Settings.


#include "TEventGameStateChange.h"

UTEventGameStateChange::UTEventGameStateChange()
{
	NewGameState = EGameInProgressState::EGINP_StateNotSpecified;
	StateTimeSeconds = 0;
	StateMessage = "";
}
