// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_MainInterface.h"
#include "CPP_Character.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "CPP_GameMode.h"
#include "KillInfo.h"

void UCPP_MainInterface::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateBindings();
}

void UCPP_MainInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCPP_MainInterface::UpdateTimer(const uint8 Minutes, const uint8 Seconds) const
{
	const FText& TextMinutes{ FText::FromString(FString::FromInt(Minutes)) };
	const FText& TextSeconds{ FText::FromString(FString::FromInt(Seconds)) };

	MinutesText->SetText(TextMinutes);
	SecondsText->SetText(TextSeconds);
}

void UCPP_MainInterface::UpdateHealthWidget(const float InHealth) const
{
	HealthBar->SetPercent(InHealth / MaxHealth);
}

void UCPP_MainInterface::UpdateArmorWidget(const float InArmor) const
{
	ArmorBar->SetPercent(InArmor / MaxArmor);
}

void UCPP_MainInterface::UpdateEquipWidget(const EWeaponType InWeaponType) const
{
	const bool bIsRifleEquip{ InWeaponType == EWeaponType::Rifle };

	RifleImage->SetOpacity(bIsRifleEquip ? 1.f : 0.5f);
	RifleAmmoBox->SetRenderOpacity(bIsRifleEquip ? 1.f : 0.5f);

	PistolImage->SetOpacity(bIsRifleEquip ? 0.5f : 1.f);
	PistolAmmoBox->SetRenderOpacity(bIsRifleEquip ? 0.5f : 1.f);

	SetCrosshairVisible(true);
}

void UCPP_MainInterface::UpdateClipWidget(const EWeaponType InWeaponType, const uint8 InClipNum) const
{
	const FText& ClipNumText{ FText::FromString(FString::FromInt(InClipNum)) };

	switch (InWeaponType)
	{
	case EWeaponType::Rifle:
	{
		RifleClip->SetText(ClipNumText);
		break;
	}
	case EWeaponType::Pistol:
	{
		PistolClip->SetText(ClipNumText);
		break;
	}
	}
}

void UCPP_MainInterface::UpdateAmmoWidget(const EWeaponType InWeaponType, const uint8 InAmmoNum) const
{
	const FText& AmmoNumText{ FText::FromString(FString::FromInt(InAmmoNum)) };

	switch (InWeaponType)
	{
	case EWeaponType::Rifle:
	{
		RifleAmmo->SetText(AmmoNumText);
		break;
	}
	case EWeaponType::Pistol:
	{
		PistolAmmo->SetText(AmmoNumText);
		break;
	}
	}
}

void UCPP_MainInterface::UpdateFireAnimation()
{
	PlayAnimation(FireAnimation);

	if (!GetWorld()->GetTimerManager().IsTimerActive(ReverseAnimation))
	{
		GetWorld()->GetTimerManager().SetTimer(ReverseAnimation, [this] { PlayAnimation(FireAnimation, 0, 1, EUMGSequencePlayMode::Reverse); }, 2.f, false);
	}
}



void UCPP_MainInterface::CreateKillInfo(const FKillInfo& KillInfo, const bool bIsParticipant) const
{
	UCPP_KillInfo* KillInfoWidget{ CreateWidget<UCPP_KillInfo>(GetOwningPlayer(),KillInfoInstance) };
	KillInfoWidget->SetKillInfo(KillInfo, bIsParticipant);

	KillInfoBox->AddChildToVerticalBox(KillInfoWidget);
}

void UCPP_MainInterface::UpdateInterface(const bool bIsVisible) const
{
	SetWidgetsVisible(bIsVisible);
}

void UCPP_MainInterface::UpdateBindings()
{
	ACPP_Character* OwnerCharacter = Cast<ACPP_Character>(GetOwningPlayerPawn());
	ACPP_PlayerController* PlayerController = Cast<ACPP_PlayerController>(GetOwningPlayer());

	OwnerCharacter->OnUpdateHealth.BindUObject(this, &UCPP_MainInterface::UpdateHealthWidget);
	OwnerCharacter->OnUpdateArmor.BindUObject(this, &UCPP_MainInterface::UpdateArmorWidget);

	OwnerCharacter->OnEquipWeapon.BindUObject(this, &UCPP_MainInterface::UpdateEquipWidget);

	OwnerCharacter->OnUpdateClip.BindUObject(this, &UCPP_MainInterface::UpdateClipWidget);
	OwnerCharacter->OnUpdateAmmo.BindUObject(this, &UCPP_MainInterface::UpdateAmmoWidget);

	OwnerCharacter->OnFireWeapon.BindUObject(this, &UCPP_MainInterface::UpdateFireAnimation);

	PlayerController->OnUpdateTimer.BindUObject(this, &UCPP_MainInterface::UpdateTimer);

	MaxHealth = OwnerCharacter->GetMaxHealth();
	MaxArmor = OwnerCharacter->GetMaxArmor();

	UpdateHealthWidget(MaxHealth);
}

void UCPP_MainInterface::SetWidgetsVisible(const bool bIsVisible) const
{
	ESlateVisibility InSlateVisibility{ bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed };

	HealthBar->SetVisibility(InSlateVisibility);
	ArmorBar->SetVisibility(InSlateVisibility);
	RifleImage->SetVisibility(InSlateVisibility);
	PistolImage->SetVisibility(InSlateVisibility);
	RifleAmmoBox->SetVisibility(InSlateVisibility);
	PistolAmmoBox->SetVisibility(InSlateVisibility);
	RifleClip->SetVisibility(InSlateVisibility);
	RifleAmmo->SetVisibility(InSlateVisibility);
	PistolClip->SetVisibility(InSlateVisibility);
	PistolAmmo->SetVisibility(InSlateVisibility);
}

void UCPP_MainInterface::SetCrosshairVisible(const bool bIsVisible) const
{
	CrosshairU->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	CrosshairD->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	CrosshairL->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	CrosshairR->SetVisibility(bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}
