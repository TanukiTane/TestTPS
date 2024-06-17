// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GameInstance.h"
#include "Engine/DataTable.h"
#include "CPP_Loot.h"

FWeaponTable* UCPP_GameInstance::GetWeaponByRowName(const FName& RowName) const
{
	FWeaponTable* WeaponInfo = WeaponTable->FindRow<FWeaponTable>(RowName, "");
	return WeaponInfo;
}

FVector UCPP_GameInstance::GetRandomSpawnPoint() const
{
	FSpawnerTable* SpawnerTable = SpawnZonesTable->FindRow<FSpawnerTable>("SpawnZones", "");
	if (SpawnerTable == nullptr) FTransform();

	return SpawnerTable->SpawnZones[FMath::RandRange(0, SpawnerTable->SpawnZones.Num() - 1)];
}
