// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_Loot.h"
#include "CPP_Armor.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API ACPP_Armor : public ACPP_Loot
{
	GENERATED_BODY()
	virtual void OnEnteredZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = ItemInfo)
	float Armor{ 0.f };
};
