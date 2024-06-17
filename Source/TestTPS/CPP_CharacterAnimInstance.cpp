// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CharacterAnimInstance.h"
#include "CPP_Character.h"

void UCPP_CharacterAnimInstance::ScrollAnimationEvent()
{
	ACPP_Character* Character{ Cast<ACPP_Character>(TryGetPawnOwner()) };
	if (Character == nullptr) return;
	Character->ChangeWeapon();
}

void UCPP_CharacterAnimInstance::FinishScrollAnimationEvent()
{
	ACPP_Character* Character{ Cast<ACPP_Character>(TryGetPawnOwner()) };
	if (Character == nullptr) return;
	Character->FinishChangeWeapon();
}

void UCPP_CharacterAnimInstance::FinishReloadAnimationEvent()
{
	ACPP_Character* Character{ Cast<ACPP_Character>(TryGetPawnOwner()) };
	if (Character == nullptr) return;
	Character->FinishReloadWeapon();
}

void UCPP_CharacterAnimInstance::FinishReceiveDamage()
{
	ACPP_Character* Character{ Cast<ACPP_Character>(TryGetPawnOwner()) };
	if (Character == nullptr) return;

	Character->FinishReceiveDamage();
}
