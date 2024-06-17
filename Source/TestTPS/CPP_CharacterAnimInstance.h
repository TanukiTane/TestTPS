// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CPP_CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TESTTPS_API UCPP_CharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ScrollAnimationEvent();

	UFUNCTION(BlueprintCallable)
	void FinishScrollAnimationEvent();

	UFUNCTION(BlueprintCallable)
	void FinishReloadAnimationEvent();

	UFUNCTION(BlueprintCallable)
	void FinishReceiveDamage();
};
