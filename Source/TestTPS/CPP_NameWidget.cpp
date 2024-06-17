// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_NameWidget.h"
#include "Components/TextBlock.h"

void UCPP_NameWidget::SetPlayerName(const FString& PlayerName) const
{
	PlayerNameText->SetText(FText::FromString(PlayerName));
}