// Fill out your copyright notice in the Description page of Project Settings.


#include "Temp/TPlayerControllerTeam.h"

ATPlayerControllerTeam::ATPlayerControllerTeam()
{
	TeamId = FGenericTeamId(10);
}

FGenericTeamId ATPlayerControllerTeam::GetGenericTeamId() const
{
	return TeamId;
}

