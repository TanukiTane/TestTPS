// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GameOver.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CPP_PlayerController.h"

void UCPP_GameOver::NativeConstruct()
{
	Super::NativeConstruct();

	GameOverButton->OnClicked.AddDynamic(this, &UCPP_GameOverWidget::OnButtonClick);
}

void UCPP_GameOver::OnButtonClick()
{
	ACPP_PlayerController* PlayerController{ GetOwningPlayer<ACPP_PlayerController>() };

	PlayerController->Client_RemoveGameOverWidget();
	PlayerController->Server_RespawnPlayer(true);
}
}

void UCPP_GameOver::SetMVPPlayer(const FString& PlayerMPV)
{
	MVPPlayer->SetText(FText::FromString(PlayerMPV));
}
