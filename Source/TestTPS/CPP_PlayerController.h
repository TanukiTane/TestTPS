// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_MainWeapon.h"
#include "GameFramework/PlayerController.h"
#include "CPP_PlayerController.generated.h"

USTRUCT()
struct FKillInfo
{
	GENERATED_BODY()

	FString KillerName{ "None" };
	FString KilledName{ "None" };

	EWeaponType WeaponType{ EWeaponType::Rifle };

	bool bIsHeadshot{ false };
};

DECLARE_DELEGATE_TwoParams(FOnUpdateTimer, uint8 /*Minutes*/, uint8 /*Seconds*/)
DECLARE_DELEGATE_OneParam(FOnUpdateKillUpdateWidget, const FKillInfo&)

UCLASS()
class TESTTPS_API ACPP_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	class UCPP_MainInterface* MainInterface{ nullptr };

	UPROPERTY()
	class UCPP_GameOver* GameOver{ nullptr };

	UPROPERTY()
	class UCPP_ScoreTable* ScoreTable{ nullptr };

	UPROPERTY()
	class UCPP_NameWidget* NameWidget{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Widget)
	TSubclassOf<UCPP_MainInterface> MainInterfaceInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Widget)
	TSubclassOf<UCPP_GameOver> GameOverInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Widget)
	TSubclassOf<UCPP_ScoreTable> ScoreTableInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Widget)
	TSubclassOf<UCPP_NameWidget> PlayerNameInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Input)
	class UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Input)
	class UInputAction* ScoreAction;

public:
	UFUNCTION(Client, Reliable)
	void Client_CreateGameOver(const FString& PlayerMPV);

	UFUNCTION(Client, Reliable)
	void Client_CreateMainInterface();

	UFUNCTION(Client, Reliable)
	void Client_CreateScoreTable();

	void ShowScoreTable();
	void CloseScoreTable();

	UFUNCTION(Client, Reliable)
	void Client_CreatePlayerName();

	UFUNCTION(Client, Reliable)
	void Client_SetPlayerNameInWidget();

	UFUNCTION(Server, Reliable)
	void Server_RespawnPlayer(const bool ResetTimer);

	UFUNCTION(Client, Reliable)
	void Client_UpdateBindings();

	UFUNCTION(Client, Reliable)
	void Client_UpdateInterface(const bool bIsVisible);

	UFUNCTION(Client, Reliable)
	void Client_RemoveInterface();

	UFUNCTION(Client, Reliable)
	void Client_RemoveGameOver();

	UFUNCTION(Client, Reliable)
	void Client_RemovePlayerName();

	UFUNCTION(Client, Reliable)
	void Client_UpdateTimers(const uint8 Minutes, const uint8 Seconds);

	void GameOver(const FString& PlayerMPV);

	UFUNCTION(Client, Reliable)
	void Client_UpdateScoreTable(const FString& InPlayerName);

	UFUNCTION(Client, Reliable)
	void Client_UpdateOnKillScoreTable(const FString& KillerName, const FString& KilledName, const EWeaponType InWeaponType, const bool bIsHeadshot, const uint8 InScore);

	void OnKillPlayer(const FKillInfo& KillInfo);

	UFUNCTION(Server, Reliable)
	void Server_OnKillPlayer(const FString& KillerName, const FString& KilledName, const EWeaponType InWeaponType, const bool bIsHeadshot);

	UFUNCTION(Server, Reliable)
	FORCEINLINE void Server_SetPlayerIndex(const uint8 InPlayerIndex);

	UFUNCTION(Server, Reliable)
	FORCEINLINE void Server_AddScore();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetPlayerName() const { return PlayerName; }

	FORCEINLINE uint8 GetPlayerScore() const { return PlayerScore; }

	FOnUpdateTimer OnUpdateTimer;
	FOnUpdateKillUpdateWidget OnUpdateKillUpdateWidget;
private:
	void SetupPlayerName();

	uint8 PlayerIndex{ 0 };

	UPROPERTY(Replicated)
	FString PlayerName{ "Player" };
	
	UPROPERTY(Replicated)
	uint8 PlayerScore{ 0 };	
};
