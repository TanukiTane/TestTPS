// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_ScoreTable.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_ScoreTable : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	void AddPlayerToTable(const FString& PlayerName);
	void UpdateScore(const FString& PlayerName, const uint8 InScore);

private:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PlayerNamesBox;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerScoresBox;

	TMap<FString, class UTextBlock*> PlayersInfo;
};
