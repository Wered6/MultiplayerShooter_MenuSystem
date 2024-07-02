// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> HostButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> JoinButton;

	// Subsystem designed to handle all online session functionality
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
};
