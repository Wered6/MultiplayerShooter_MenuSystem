// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch)
{
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

void UMenu::HostButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Host Button Clicked"))
		);
	}

#pragma region Nullchecks
	if (!MultiplayerSessionsSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|MultiplayerSessionsSubsystem is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);

	UWorld* World{GetWorld()};

#pragma region Nullchecks
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s|World is nullptr"), *FString(__FUNCTION__))
		return;
	}
#pragma endregion

	World->ServerTravel("/Game/ThirdPerson/Maps/Lobby?listen");
}

void UMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Join Button Clicked"))
		);
	}
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
