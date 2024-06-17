// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_MainWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "CPP_Bullet.h"

// Sets default values
ACPP_MainWeapon::ACPP_MainWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void ACPP_MainWeapon::BeginPlay()
{
	Super::BeginPlay();

	ClipNum = ClipMax;
	AmmoNum = AmmoMax;
}

// Called every frame
void ACPP_MainWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_MainWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPP_MainWeapon, MeshComponent);
}

void ACPP_MainWeapon::Server_SetWeaponType_Implementation(const EWeaponType InWeaponType)
{
	Multicast_SetWeaponType(InWeaponType);
}

void ACPP_MainWeapon::Multicast_SetWeaponType_Implementation(const EWeaponType InWeaponType)
{
	WeaponType = InWeaponType;
}

void ACPP_MainWeapon::Fire(const FVector& DirectionShot, const FString& PlayerName)
{
	const FVector& SocketsLocation{ MeshComponent->GetSocketLocation(BulletSocket) };
	const FRotator& SocketsRotation{ MeshComponent->GetSocketRotation(BulletSocket) };

	ACPP_Bullet* Projectile{ GetWorld()->SpawnActor<ACPP_Bullet>(BulletInstance, SocketsLocation, SocketsRotation) };
	if (Bullet == nullptr) return;

	Bullet->SetProjectileDirection(DirectionShot);
	Bullet->SetInstigatorName(PlayerName, WeaponType);

	MeshComponent->PlayAnimation(FireAnim, false);

	--AmmoNum;
}

void ACPP_MainWeapon::Reload()
{
	--ClipNum;
	AmmoNum = AmmoMax;
}

void ACPP_MainWeapon::AddAmmoToWeapon()
{
	if (AmmoNum == AmmoMax)
	{
		if (ClipNum < ClipMax)
			++ClipNum;
	}
	else
	{
		AmmoNum = AmmoMax;
	}
}

