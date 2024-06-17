// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_MainWeapon.h"
#include "CPP_Bullet.generated.h"

UCLASS()
class TESTTPS_API ACPP_Bullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Bullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetBulletDirection(const FVector& BulletDirection) const;
	void SetInstigatorName(const FString& InInstigatorName, const EWeaponType InWeaponType);

private:
	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BulletMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent{ nullptr };
	
	FString InstigatorName{ "None" };

	EWeaponType WeaponType{ EWeaponType::Rifle };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Damage)
	float Damage{ 0.f };
};
