// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GameMode.h"
#include "CPP_GameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ACPP_GameMode::ACPP_GameMode()
{
	LatentActionInfo = FLatentActionInfo{ 0, 0, TEXT("UpdateTimer"), this };
}

void ACPP_GameMode::SpawnPlayerInRandomPlace(ACPP_PlayerController* PlayerController)
{
	PlayerController->Possess(SpawnDefaultPawnAtTransform(PlayerController, GetRandomSpawnTransform()));
}

void ACPP_GameMode::AddListener(ACPP_PlayerController* PlayerController)
{
	PlayersController.Emplace(PlayerController);

	PlayerController->Server_SetPlayerIndex(PlayersController.Num());
	PlayerController->OnUpdateKillUpdateWidget.BindUObject(this, &ACPP_GameMode::UpdatePlayersScore);
}

void ACPP_GameMode::UpdateTimers()
{
	if (!bIsUpdateTimer)
	{
		Minutes = TimerMinutes;
		Seconds = TimerSeconds;
		bIsUpdateTimer = true;
	}
}

void ACPP_GameMode::AddNamePlayerToTable(const FString& PlayerName)
{
	PlayerNames.Emplace(PlayerName);
	for (const FString& Name : PlayerNames)
	{
		for (ACPP_PlayerController* PlayerController : PlayersController)
		{
			PlayerController->Client_UpdateScoreTable(Name);
		}
	}
}

void ACPP_GameMode::UpdatePlayersScore(const FKillInfo& KillInfo)
{
	const uint8 InScore{ GetPlayerScore(KillInfo.KillerName) };
	for (ACPP_PlayerController* PlayerController : PlayersController)
	{
		PlayerController->Client_UpdateOnKillScoreTable(KillInfo.KillerName, KillInfo.KilledName, KillInfo.WeaponType, KillInfo.bIsHeadshot, InScore);
	}
}

void ACPP_GameMode::BeginPlay()
{
	Super::BeginPlay();

	UpdateTimers();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnActors);

	FTimerHandle TimerSpawn;
	GetWorld()->GetTimerManager().SetTimer(TimerSpawn, [this] { SpawnRandomPickups(); }, 10.f, false);
}

void ACPP_GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UKismetSystemLibrary::Delay(GetWorld(), 1.f, LatentActionInfo);
}

void ACPP_GameMode::SpawnRandomPickups()
{
	const int32 RandomLoot{ FMath::RandRange(0, SpawnPickups.Num() - 1) };
	const FVector RandomSpawnPoint{ GetRandomPickupSpawn() };

	AActor* SpawnActor = GetWorld()->SpawnActor<ACPP_Loot>(SpawnPickups[RandomLoot], RandomSpawnPoint, FRotator{});
	if (SpawnActor)
	{
		SpawnActor->SetReplicates(true);
		++PickupsNum;
	}

	if (PickupsNum < SpawnPickups.Num())
	{
		const float RandomTimer{ FMath::RandRange(10.f, 30.f) };

		FTimerHandle TimerSpawn;
		GetWorld()->GetTimerManager().SetTimer(TimerSpawn, [this] { SpawnRandomPickups(); }, RandomTimer, false);
	}
}

const FTransform ACPP_GameMode::GetRandomSpawnTransform() const
{
	const int32 RandomZone{ FMath::RandRange(0, SpawnActors.Num() - 1) };
	return SpawnActors[RandomZone]->GetTransform();
}

const FVector ACPP_GameMode::GetRandomPickupSpawn() const
{
	const UCPP_GameInstance* GameInstance{ GetGameInstance<UCPP_GameInstance>() };
	if (GameInstance == nullptr) return FVector();
	return GameInstance->GetRandomSpawnPoint();
}

void ACPP_GameMode::GameOver()
{
	uint8 BestScore{ 0 };
	FString MVPPlayer{ "None" };
	for (ACPP_PlayerController* PlayerController : PlayersController)
	{
		if (PlayerController->GetPlayerScore() > BestScore)
		{
			BestScore = PlayerController->GetPlayerScore();
			MVPPlayer = PlayerController->GetPlayerName();
		}
	}
	for (ACPP_PlayerController* PlayerController : PlayersController)
	{
		PlayerController->GameOver(MVPPlayer);
	}
	bIsUpdateTimer = false;
}

void ACPP_GameMode::UpdateTimer()
{
	if (Seconds == 0)
	{
		if (Minutes == 0)
		{
			GameOver();
			return;
		}

		Seconds = 59.f;
		--Minutes;
	}
	else
		--Seconds;

	for (ACPP_PlayerController* PlayerController : PlayersController)
	{
		PlayerController->Client_UpdateTimers(Minutes, Seconds);
	}
}

uint8 ACPP_GameMode::GetPlayerScore(const FString& PlayerName) const
{
	for (ACPP_PlayerController* PlayerController : PlayersController)
	{
		if (PlayerController->GetPlayerName() == PlayerName)
		{
			PlayerController->Server_AddScore();
			return PlayerController->GetPlayerScore();
		}
	}
	return 0;
}
