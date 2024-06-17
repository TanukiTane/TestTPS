// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Ammo.h"
#include "CPP_Character.h"

void ACPP_Ammo::OnEnteredZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnEnteredZone(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (GetNetMode() != NM_DedicatedServer)
	{
		ACPP_Character* EnteredCharacter{ Cast<ACPP_Character>(OtherActor) };
		if (EnteredCharacter == nullptr) return;

		EnteredCharacter->AddAmmo(WeaponType);
	}
	Destroy();
}
