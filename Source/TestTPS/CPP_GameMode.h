// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CPP_PlayerController.h"
#include "CPP_Loot.h"
#include "CPP_GameMode.generated.h"

UCLASS()
class TESTTPS_API ACPP_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACPP_GameMode();

	void SpawnPlayerInRandomPlace(ACPP_PlayerController* PlayerController);
	void AddListener(ACPP_PlayerController* PlayerController);
	void UpdateTimers();

	void AddNamePlayerToTable(const FString& PlayerName);
	void UpdatePlayersScore(const FKillInfo& KillInfo);

	static uint8 PickupsNum;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void SpawnRandomPickups();

	const FTransform GetRandomSpawnTransform() const;
	const FVector GetRandomPickupSpawn() const;
	void GameOver();

	UFUNCTION()
	void UpdateTimer();

	uint8 GetPlayerScore(const FString& PlayerName) const;

	FLatentActionInfo LatentActionInfo;

	uint8 Seconds{ 0 };
	uint8 Minutes{ 0 };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = Timer)
	uint8 TimerSeconds{ 0 };
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = Timer)
	uint8 TimerMinutes{ 5 };

	UPROPERTY()
	TArray<AActor*> SpawnActors;

	UPROPERTY()
	TArray<ACPP_PlayerController*> PlayersController;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = Spawn)
	TArray<TSubclassOf<ACPP_Loot>> SpawnPickups;

	TArray<FString> PlayerNames;

	bool bIsUpdateTimer{ false };
};

uint8 ACPP_GameMode::PickupsNum = 0;