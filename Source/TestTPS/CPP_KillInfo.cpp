// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_KillInfo.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerController.h"

void UCPP_KillInfo::NativeConstruct()
{
	Super::NativeConstruct();	
}

void UCPP_KillInfo::SetKillInfo(const FKillInfo& KillInfo, const bool bIsParticipant)
{
	KillPlayerText->SetText(FText::FromString(KillInfo.KillerName));
	KilledPlayerText->SetText(FText::FromString(KillInfo.KilledName));

	GunImage->SetBrushFromTexture(*WeaponTextures.Find(KillInfo.WeaponType));

	if (bIsParticipant)
		OwnerImageKill->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (KillInfo.bIsHeadshot)
	{
		HeadshotImage->SetVisibility(ESlateVisibility::HitTestInvisible);

		UHorizontalBoxSlot* HeadshotImageSlot{ GunBox->AddChildToHorizontalBox(HeadshotImage) };
		HeadshotImageSlot->SetPadding(FMargin{ 15.0, 0.0, 0.0, 0.0 });
		HeadshotImageSlot->SetVerticalAlignment(VAlign_Center);
	}
}

void UCPP_KillInfo::FinishAnimation()
{
	RemoveFromParent();
}
