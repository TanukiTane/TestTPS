// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CPP_MainWeapon.h"
#include "CPP_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Tables)
	UDataTable* WeaponTable{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Tables)
	UDataTable* SpawnZonesTable{ nullptr };

public:
	FWeaponTable* GetWeaponByRowName(const FName& RowName) const;
	FVector GetRandomSpawnPoint() const;

};
