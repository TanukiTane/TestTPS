// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Loot.h"
#include "Components/BoxComponent.h"
#include "CPP_GameMode.h"

// Sets default values
ACPP_Loot::ACPP_Loot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("EnteredArea");
	SetRootComponent(BoxCollision);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(GetRootComponent());

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACPP_LootableActor::OnEnteredZone);
}

// Called when the game starts or when spawned
void ACPP_Loot::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ACPP_Loot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SkeletalMeshComponent)
	{
		const FRotator& SkeletalMeshRotator{ SkeletalMeshComponent->GetComponentRotation() };
		const FVector RotatedVector{ SkeletalMeshRotator.Vector().RotateAngleAxis(1.0, FVector{ 0.0, 0.0, 1.0 }) };

		SkeletalMeshComponent->SetWorldRotation(RotatedVector.Rotation());
	}

	if (StaticMeshComponent)
	{
		const FRotator& StaticMeshRotator{ StaticMeshComponent->GetComponentRotation() };
		const FVector RotatedVector{ StaticMeshRotator.Vector().RotateAngleAxis(1.0, FVector{ 0.0, 0.0, 1.0 }) };

		StaticMeshComponent->SetWorldRotation(RotatedVector.Rotation());
	}
}

void ACPP_Loot::OnEnteredZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACPP_GameMode::PickupsNum != 0)
		ACPP_GameMode::PickupsNum--;	
}

