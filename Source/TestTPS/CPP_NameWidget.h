// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_NameWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_NameWidget : public UUserWidget
{
	GENERATED_BODY()	

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;

public:
	void SetPlayerName(const FString& PlayerName) const;
};
