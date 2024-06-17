// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CPP_Character.h"
#include "CPP_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACPP_Bullet::ACPP_Bullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SetRootComponent(SphereCollision);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	BulletMesh->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->InitialSpeed = 5000.f;

	SphereCollision->OnComponentHit.AddDynamic(this, &ACPP_Bullet::OnBulletHit);
}

// Called when the game starts or when spawned
void ACPP_Bullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPP_Bullet::SetBulletDirection(const FVector& BulletDirection) const
{
	const FVector& DirectionShot{ BulletDirection - GetActorLocation() };
	ProjectileMovementComponent->Velocity = DirectionShot.GetSafeNormal() * ProjectileMovementComponent->InitialSpeed;
}

void ACPP_Bullet::SetInstigatorName(const FString& InInstigatorName, const EWeaponType InWeaponType)
{
	InstigatorName = InInstigatorName;
	WeaponType = InWeaponType;
}

void ACPP_Bullet::OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACPP_Character* HitCharacter{ Cast<ACPP_Character>(OtherActor) };

	if (HitCharacter != nullptr)
	{
		FKillInfo KillInfo{ InstigatorName, "None", WeaponType, Hit.BoneName == "head" };

		HitCharacter->ApplyDamage(Damage, KillInfo);		
	}		

	Destroy();
}

