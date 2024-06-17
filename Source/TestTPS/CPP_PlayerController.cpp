// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CPP_MainInterface.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "CPP_Character.h"
#include "Components/WidgetComponent.h"
#include "CPP_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "CPP_GameOver.h"
#include "CPP_NameWidget.h"
#include "CPP_ScoreTable.h"

void ACPP_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	Client_CreateScoreTable();
	Client_CreateMainInterface();

	ACPP_GameMode* GameMode{ Cast<ACPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld())) };
	if (GameMode == nullptr) return;

	GameMode->AddListener(this);

	PlayerName = PlayerName + " " + FString::FromInt(PlayerIndex);
	GameMode->AddNamePlayerToTable(PlayerName);
}

void ACPP_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ScoreAction, ETriggerEvent::Started, this, &ACPP_PlayerController::ShowScoreTable);
		EnhancedInputComponent->BindAction(ScoreAction, ETriggerEvent::Completed, this, &ACPP_PlayerController::CloseScoreTable);
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(BaseMappingContext, 0);
	}
}

void ACPP_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPP_PlayerController, PlayerName);
	DOREPLIFETIME(ACPP_PlayerController, PlayerScore);
}

void ACPP_PlayerController::Client_CreateGameOver_Implementation(const FString& PlayerMPV)
{
	GameOver = CreateWidget<UCPP_GameOver>(this, GameOverInstance, "GameOverWidget");
	GameOver->AddToViewport();
	GameOver->SetMVPPlayer(PlayerMPV);
	SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, GameOver, EMouseLockMode::DoNotLock, true);
}

void ACPP_PlayerController::Client_CreateMainInterface()
{
	MainInterface = CreateWidget<UCPP_MainInterface>(this, MainInterfaceInstance, "MainInterfaceWidget");
	MainInterface->AddToViewport();

	SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this, true);
}

void ACPP_PlayerController::Client_CreateScoreTable_Implementation()
{
	if (ScoreTable == nullptr)
	{
		ScoreTable = CreateWidget<UCPP_ScoreTable>(this, ScoreTableInstance, "ScoreTableWidget");
		ScoreTable->SetVisibility(ESlateVisibility::Collapsed);
		ScoreTable->AddToViewport();
	}
}

void ACPP_PlayerController::ShowScoreTable()
{
	ScoreTable->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void ACPP_PlayerController::CloseScoreTable()
{
	ScoreTable->SetVisibility(ESlateVisibility::Collapsed);
}

void ACPP_PlayerController::Client_CreatePlayerName_Implementation()
{
	PlayerName = CreateWidget<UCPP_NameWidget>(this, PlayerNameInstance, "PlayerNameWidget");
	const ACPP_Character* ControlledCharacter{ Cast<ACPP_Character>(GetPawn()) };
	ControlledCharacter->GetPlayerNameWidgetComponent()->SetWidget(PlayerName);
}

void ACPP_PlayerController::Client_SetPlayerNameInWidget_Implementation()
{
	PlayerName->SetPlayerName(PlayerName);
}

void ACPP_PlayerController::Server_RespawnPlayer_Implementation(const bool ResetTimer)
{
	SetPause(false);
	SetShowMouseCursor(false);

	if (GetPawn())
		GetPawn()->Destroy();

	if (HasAuthority())
	{
		ACPP_GameMode* GameMode{ Cast<ACPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld())) };
		if (GameMode == nullptr) return;

		GameMode->SpawnPlayerInRandomPlace(this);

		if (ResetTimer)
			GameMode->UpdateTimers();
	}
}

void ACPP_PlayerController::Client_UpdateBindings_Implementation()
{
	if (MainInterface)
	{
		MainInterface->UpdateBindings();
	}
}

void ACPP_PlayerController::Client_UpdateInterface_Implementation(const bool bIsVisible)
{
	if (MainInterface)
	{
		MainInterface->UpdateInterface(bIsVisible);
	}
}

void ACPP_PlayerController::Client_RemoveInterface_Implementation ()
{
	if (MainInterface)
	{
		MainInterface->RemoveFromParent();
		MainInterface = nullptr;
	}
}

void ACPP_PlayerController::Client_RemoveGameOver_Implementation()
{
	if (GameOver)
	{
		GameOver->RemoveFromParent();
		GameOver = nullptr;
	}
}

void ACPP_PlayerController::Client_RemovePlayerName_Implementation()
{
	if (PlayerName)
	{
		PlayerName->RemoveFromParent();
		PlayerName = nullptr;
	}
	if (MainInterface)
		MainInterface->SetCrosshairVisible(false);
}

void ACPP_PlayerController::Client_UpdateTimers_Implementation(const uint8 Minutes, const uint8 Seconds)
{
	if (OnUpdateTimer.IsBound())
	{
		OnUpdateTimer.Execute(Minutes, Seconds);
	}
}

void ACPP_PlayerController::GameOver(const FString& PlayerMPV)
{
	ACPP_Character* PossessedCharacter{ Cast<ACPP_Character>(GetPawn()) };
	if (PossessedCharacter)
	{
		PossessedCharacter->UpdateEssentials();
		PossessedCharacter->Destroy();
	}

	PlayerScore = 0;

	SetPause(true);

	Client_RemoveInterface();
	Client_RemovePlayerName();

	Client_CreateGameOver(PlayerMPV);
}

void ACPP_PlayerController::Client_UpdateScoreTable_Implementation(const FString& InPlayerName)
{
	if (ScoreTable)
	{

		ScoreTable->AddPlayerToTable(InPlayerName);
	}
	else
	{

	}
}

void ACPP_PlayerController::Client_UpdateOnKillScoreTable_Implementation(const FString& KillerName, const FString& KilledName, const EWeaponType InWeaponType, const bool bIsHeadshot, const uint8 InScore)
{
	if (ScoreTable)
	{
		ScoreTable->UpdateScore(KillerName, InScore);
	}
	if (MainInterface)
	{
		MainInterface->CreateKillInfo({ KillerName, KilledName, InWeaponType, bIsHeadshot }, (KillerName == PlayerName || KilledName == PlayerName));
	}
}

void ACPP_PlayerController::OnKillPlayer(const FKillInfo& KillInfo)
{
	Server_OnKillPlayer(KillInfo.KillerName, PlayerName, KillInfo.WeaponType, KillInfo.bIsHeadshot);
}

void ACPP_PlayerController::Server_OnKillPlayer_Implementation(const FString& KillerName, const FString& KilledName, const EWeaponType InWeaponType, const bool bIsHeadshot)
{
	OnUpdateKillUpdateWidget.Execute({ KillerName, KilledName, InWeaponType, bIsHeadshot });
}

void ACPP_PlayerController::Server_SetPlayerIndex_Implementation(const uint8 InPlayerIndex)
{
	PlayerIndex = InPlayerIndex;
}

void ACPP_PlayerController::Server_AddScore_Implementation()
{
	++PlayerScore;
}

void ACPP_PlayerController::SetupPlayerName()
{
	ACPP_GameMode* GameMode{ Cast<ACPP_GameMode>(UGameplayStatics::GetGameMode(GetWorld())) };
	if (GameMode == nullptr) return;
}
