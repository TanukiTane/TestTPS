// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_MainWeapon.h"
#include "CPP_MainInterface.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_MainInterface : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateTimer(const uint8 Minutes, const uint8 Seconds) const;

	UFUNCTION()
	void UpdateHealthWidget(const float InHealth) const;

	UFUNCTION()
	void UpdateArmorWidget(const float InArmor) const;

	UFUNCTION()
	void UpdateEquipWidget(const EWeaponType InWeaponType) const;

	UFUNCTION()
	void UpdateClipWidget(const EWeaponType InWeaponType, const uint8 InClipNum) const;

	UFUNCTION()
	void UpdateAmmoWidget(const EWeaponType InWeaponType, const uint8 InAmmoNum) const;

	UFUNCTION()
	void UpdateFireAnimation();

public:
	void CreateKillInfo(const struct FKillInfo& KillInfo, const bool bIsParticipant) const;

	void UpdateInterface(const bool bIsVisible) const;
	void UpdateBindings();

	void SetWidgetsVisible(const bool bIsVisible) const;
	void SetCrosshairVisible(const bool bIsVisible) const;
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ArmorBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* RifleImage;

	UPROPERTY(meta = (BindWidget))
	UImage* PistolImage;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* RifleAmmoBox;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* PistolAmmoBox;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* KillInfoBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RifleClip;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RifleAmmo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PistolClip;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PistolAmmo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MinutesText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondsText;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairU;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairD;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairL;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairR;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FireAnimation;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = InfoWidget)
	TSubclassOf<class UCPP_KillInfo> KillInfoInstance;

	FTimerHandle ReverseAnimation;

	float MaxHealth{ 0.f };
	float MaxArmor{ 0.f };
};
