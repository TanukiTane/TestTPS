// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_GameOver.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_GameOver : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClick();

public:
	void SetMVPPlayer(const FString& PlayerMPV);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* GameOverButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MVPPlayer;
};
