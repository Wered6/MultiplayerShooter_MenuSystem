// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	const UWorld* World{GetWorld()};

#pragma region Nullchecks
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|World is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	APlayerController* PlayerController{World->GetFirstPlayerController()};

#pragma region Nullchecks
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|PlayerController is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(true);

	const UGameInstance* GameInstance{GetGameInstance()};

#pragma region Nullchecks
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|GameInstance is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

#pragma region Nullchecks
	if (!MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|MultiplayerSessionsSubsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::OnCreateSession);
	MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSessions);
	MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

#pragma region Nullchecks
	if (!HostButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|HostButton is nullptr"), *FString(__FUNCTION__))
		return false;
	}
	if (!JoinButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|JoinButton is nullptr"), *FString(__FUNCTION__))
		return false;
	}
#pragma endregion

	HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session created successfully!"))
			);
		}
		UWorld* World{GetWorld()};

#pragma region Nullchecks
		if (!World)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s|World is nullptr"), *FString(__FUNCTION__))
			return;
		}
#pragma endregion

		World->ServerTravel(PathToLobby);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session"))
			);
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
#pragma region Nullchecks
	if (!MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|MultiplayerSessionsSubsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	for (FOnlineSessionSearchResult Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.IsEmpty())
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSubsystem* Subsystem{IOnlineSubsystem::Get()};

#pragma region Nullchecks
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|Subsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	const IOnlineSessionPtr SessionInterface{Subsystem->GetSessionInterface()};

	if (SessionInterface.IsValid())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

		APlayerController* PlayerController{GetGameInstance()->GetFirstLocalPlayerController()};

#pragma region Nullchecks
		if (!PlayerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s|PlayerController is nullptr"), *FString(__FUNCTION__))
			return;
		}
#pragma endregion

		PlayerController->ClientTravel(Address, TRAVEL_Absolute);
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
#pragma region Nullchecks
	if (!MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|MultiplayerSessionsSubsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	HostButton->SetIsEnabled(false);

	MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

void UMenu::JoinButtonClicked()
{
#pragma region Nullchecks
	if (!MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|MultiplayerSessionsSubsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	JoinButton->SetIsEnabled(false);

	MultiplayerSessionsSubsystem->FindSessions(10000);
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	const UWorld* World{GetWorld()};

#pragma region Nullchecks
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|World is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	APlayerController* PlayerController{World->GetFirstPlayerController()};

#pragma region Nullchecks
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|PlayerController is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	const FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(false);
}
