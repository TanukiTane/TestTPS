// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "CPP_MainWeapon.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	Rifle UMETA(DisplayName = "Rifle"),
	Pistol UMETA(DisplayName = "Pistol")
};

USTRUCT(BlueprintType)
struct FWeaponTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType{ EWeaponType::Rifle };

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACPP_MainWeapon> WeaponInstance;
};

UCLASS()
class TESTTPS_API ACPP_MainWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_MainWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE void SetWeaponType(const EWeaponType InWeaponType) { Server_SetWeaponType(InWeaponType); }

	UFUNCTION(Server, Reliable)
	void Server_SetWeaponType(const EWeaponType InWeaponType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWeaponType(const EWeaponType InWeaponType);

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE float GetFireRate() const { return FireRate; }

	FORCEINLINE bool IsFullAmmo() const { return AmmoNum == AmmoMax; }
	FORCEINLINE bool IsAmmoEmpty() const { return AmmoNum == 0; }
	FORCEINLINE bool IsClipEmpty() const { return ClipNum == 0; }

	FORCEINLINE int32 GetMaxClip() const { return ClipMax; }
	FORCEINLINE int32 GetMaxAmmo() const { return AmmoMax; }

	FORCEINLINE int32 GetCurrentClip() const { return ClipNum; }
	FORCEINLINE int32 GetCurrentAmmo() const { return AmmoNum; }

	void Fire(const FVector& DirectionShot, const FString& PlayerName);	
	
	void Reload();
	void AddAmmoToWeapon();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"), Category = Component)
	USkeletalMeshComponent* MeshComponent{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Essentials)
	EWeaponType WeaponType{ EWeaponType::Rifle };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Essentials)
	TSubclassOf<class ACPP_Bullet> BulletInstance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Essentials)
	FName BulletSocket{ "None" };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	UAnimationAsset* FireAnim{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	USoundBase* SoundReload{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	float FireRate{ 0.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	int32 AmmoMax{ 0 };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	int32 ClipMax{ 0 };

	int32 AmmoNum{ 0 };
	int32 ClipNum{ 0 };
};
