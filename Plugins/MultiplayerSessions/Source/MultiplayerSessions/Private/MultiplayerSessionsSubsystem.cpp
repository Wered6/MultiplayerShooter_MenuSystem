// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	IOnlineSubsystem* Subsystem{IOnlineSubsystem::Get()};

#pragma region Nullchecks
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|Subsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	SessionInterface = Subsystem->GetSessionInterface();
}
