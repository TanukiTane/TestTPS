// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_MainWeapon.h"
#include "CPP_KillInfo.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_KillInfo : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	void SetKillInfo(const struct FKillInfo& KillInfo, const bool bIsParticipant);

private:
	UFUNCTION()
	void FinishAnimation();

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* KillBox;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* GunBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KillPlayerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledPlayerText;

	UPROPERTY(meta = (BindWidget))
	class UImage* GunImage;

	UPROPERTY(meta = (BindWidget))
	UImage* HeadshotImage;

	UPROPERTY(meta = (BindWidget))
	UImage* OwnerImageKill;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* DisappearAnimation;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Images)
	TMap<EWeaponType, UTexture2D*> WeaponTextures;

	bool bIsStartDisappear{ false };

};
