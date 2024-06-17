// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "CPP_PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "CPP_GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "CPP_MainWeapon.h"


// Sets default values
ACPP_Character::ACPP_Character()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // rotation rate	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;	
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CameraTimeline = CreateDefaultSubobject<UTimelineComponent>("TimelineComponent");

	PlayerNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("WidgetComponent");
	PlayerNameWidgetComponent->SetupAttachment(GetMesh());
	PlayerNameWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

}

// Called when the game starts or when spawned
void ACPP_Character::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat OnTimelineCallback;
	FOnTimelineEventStatic OnTimelineEventStatic;

	OnTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
	OnTimelineEventStatic.BindUFunction(this, FName{ TEXT("TimelineEventFinish") });

	if (FloatCurve)
	{
		CameraTimeline->AddInterpFloat(FloatCurve, OnTimelineCallback);
		CameraTimeline->SetTimelineFinishedFunc(OnTimelineEventStatic);
	}

	ACPP_PlayerController* PlayerController{ GetController<ACPP_PlayerController>() };
	if (PlayerController == nullptr) return;

	PlayerController->Client_CreatePlayerName();
	PlayerController->Client_UpdateBindings();
	PlayerController->Client_UpdateInterface(true);

	PlayerController->Client_SetPlayerNameInWidget();

	bIsDeath = false;	
}

// Called every frame
void ACPP_Character::Tick(float DeltaSeconds))
{
	Super::Tick(DeltaSeconds);
}

void ACPP_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPP_Character, CalculatedPitchRotator);
	DOREPLIFETIME(ACPP_Character, CameraTimeline);
	DOREPLIFETIME(ACPP_Character, BlendAlphaWeapon);
}

void ACPP_Character::Move(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);

	DirectionMoveX = MovementVector.X;
	DirectionMoveY = MovementVector.Y;

	Server_SetDirectionX(MovementVector.X);
	Server_SetDirectionY(MovementVector.Y);
}

void ACPP_Character::Look(const FInputActionValue& Value)
{
	if (Controller == nullptr) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

	CalculatePitchRotator();
}

void ACPP_Character::AimStart(const FInputActionValue& Value)
{
	if (!bIsEquipWeapon || GetIsReload() || GetIsScrollWeapon()) return;

	SetAimMode(true);
	Server_Aim(true);
}

void ACPP_Character::AimEnd(const FInputActionValue& Value)
{
	if (!bIsEquipWeapon) return;

	SetAimMode(false);
	Server_Aim(false);
}

void ACPP_Character::Fire(const FInputActionValue& Value)
{
	if (!bIsEquipWeapon || GetIsReload() || GetIsScrollWeapon() || !bIsFireReady
		|| CombatWeapon->IsAmmoEmpty())
		return;

	const FVector& StartTrace{ FollowCamera->GetComponentLocation() };
	const FVector& EndTrace{ StartTrace + FollowCamera->GetComponentRotation().Vector() * 10000.f };

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_MAX);

	FVector ImpactPoint{ 0.0 };
	if (HitResult.bBlockingHit)
	{
		ImpactPoint = HitResult.ImpactPoint;
	}
	else
	{
		ImpactPoint = EndTrace;
	}

	ACPP_PlayerController* PlayerController{ GetController<ACPP_PlayerController>() };
	if (PlayerController)
	{
		const FString& PlayerName{ PlayerController->GetPlayerName() };

		CombatWeapon->Fire(ImpactPoint, PlayerName);

		OnFireWeapon.Execute();
		OnUpdateAmmo.Execute(CombatWeapon->GetWeaponType(), CombatWeapon->GetCurrentAmmo());

		SetFireRate();

		Server_Fire(ImpactPoint, PlayerName);
	}
}

void ACPP_Character::Reload(const FInputActionValue& Value)
{
	if (!bIsEquipWeapon || GetIsReload() || GetIsAimWeapon() || GetIsScrollWeapon()
		|| CombatWeapon->IsFullAmmo() || CombatWeapon->IsClipEmpty())
		return;

	bIsReload = true;
	CombatWeapon->Reload();
	Server_Reload(true);
}

void ACPP_Character::ScrollWeapon(const FInputActionValue& Value)
{
	if (IsInvalidAction()) return;

	bIsScrollWeapon = true;
	Server_Scroll(true);
}

void ACPP_Character::AddWeaponMappingContext() const
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(WeaponMappingContext, WeaponMappingContextPriority);
		}
	}
}

void ACPP_Character::Server_SetDirectionX_Implementation(const float XValue)
{
	Multicast_SetDirectionX(XValue);
}

void ACPP_Character::Multicast_SetDirectionX_Implementation(const float XValue)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer) return;
	DirectionMoveX = XValue;
}

void ACPP_Character::Server_SetDirectionY_Implementation(const float YValue)
{
	Multicast_SetDirectionY(YValue);
}

void ACPP_Character::Multicast_SetDirectionY_Implementation(const float YValue)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer) return;
	DirectionMoveY = YValue;
}

void ACPP_Character::CalculatePitchRotator()
{
	const FRotator& ControlRotation{ GetControlRotation() };

	const double Pitch
	{
		ControlRotation.Pitch > 180.0 ? (360.0 - ControlRotation.Pitch) : (ControlRotation.Pitch * (-1.0))
	};

	CalculatedPitchRotator = { 0.0, 0.0, Pitch / 5.0 };
	Server_SetCalculatedPitchRotator(CalculatedPitchRotator);
}

void ACPP_Character::Server_SetCalculatedPitchRotator_Implementation(const FRotator& InRotator)
{
	Multicast_SetCalculatedPitchRotator(InRotator);
}

void ACPP_Character::Multicast_SetCalculatedPitchRotator_Implementation(const FRotator& InRotator)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer) return;
	CalculatedPitchRotator = InRotator;
}

void ACPP_Character::SetAimMode(const bool Value)
{
	bIsAimWeapon = Value;
	PlayTimeline(Value);
	GetCharacterMovement()->MaxWalkSpeed = Value ? 250.f : 500.f;
}

void ACPP_Character::Server_Aim_Implementation(const bool Value)
{
	Multicast_Aim(Value);
	GetCharacterMovement()->MaxWalkSpeed = Value ? 250.f : 500.f;
}

void ACPP_Character::Multicast_Aim_Implementation(const bool Value)
{
	if (GetLocalRole() != ROLE_AutonomousProxy)
	{
		bIsAimWeapon = Value;
		GetCharacterMovement()->MaxWalkSpeed = Value ? 250.f : 500.f;
	}
}

void ACPP_Character::Server_Fire_Implementation(const FVector& ProjectileDirection, const FString& PlayerName)
{
	Multicast_Fire(ProjectileDirection, PlayerName);
}

void ACPP_Character::Multicast_Fire_Implementation(const FVector& ProjectileDirection, const FString& PlayerName)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer) return;
	CombatWeapon->Fire(ProjectileDirection, PlayerName);
}

void ACPP_Character::SetFireRate()
{
	bIsFireReady = false;
	FTimerHandle FireRateHandle;
	GetWorld()->GetTimerManager().SetTimer(FireRateHandle, [this] { bIsFireReady = true; }, CombatWeapon->GetFireRate(), false);
}

void ACPP_Character::Server_Reload_Implementation(const bool Value)
{
	Multicast_Reload(Value);
}

void ACPP_Character::Multicast_Reload_Implementation(const bool Value)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer) return;
	bIsReload = Value;
	CombatWeapon->Reload();
}

const bool ACPP_Character::IsInvalidAction() const
{
	return GetIsScrollWeapon()
		|| GetIsReload()
		|| GetIsAimWeapon()
		|| EquipState != EEquipState::FullEquipped
		|| !bIsEquipWeapon;
}

void ACPP_Character::Server_Scroll_Implementation(const bool Value)
{
	Multicast_Scroll(Value);
}

void ACPP_Character::Multicast_Scroll_Implementation(const bool Value)
{
	if (GetLocalRole() == ROLE_AutonomousProxy || GetNetMode() == NM_DedicatedServer) return;
	bIsScrollWeapon = Value;
}

void ACPP_Character::PlayTimeline(bool bIsFromStart) const
{
	bIsFromStart ? CameraTimeline->PlayFromStart() : CameraTimeline->ReverseFromEnd();
}

void ACPP_Character::TimelineCallback(float Value)
{
	float TargetArmOffset;

	if (EquipState == EEquipState::None)
	{
		TargetArmOffset = FMath::Lerp(400.f, 300.f, Value);

		const double OffsetCamera = FMath::Lerp(0.0, 75.0, Value);
		CameraBoom->SetRelativeLocation(FVector{ 0.0, OffsetCamera, 0.0 });

		BlendAlphaWeapon = FMath::Lerp(0.f, 1.f, Value);
	}
	else
	{
		TargetArmOffset = FMath::Lerp(300.f, 100.f, Value);
	}
	CameraBoom->TargetArmLength = TargetArmOffset;
}

void ACPP_Character::TimelineEventFinish()
{
	if (EquipState == EEquipState::None)
	{
		EquipState = EEquipState::Equipped;
	}
}

void ACPP_Character::TryUpdateWeaponWidgets() const
{
	if (!OnEquipWeapon.IsBound()) return;
	OnEquipWeapon.Execute(CombatWeapon->GetWeaponType());
	TryUpdateAmmoClipWidgets(CombatWeapon->GetWeaponType(), CombatWeapon->GetCurrentClip(), CombatWeapon->GetCurrentAmmo());
}

void ACPP_Character::TryUpdateAmmoClipWidgets(const EWeaponType InWeaponType, const uint8 InClip, const uint8 InAmmo) const
{
	if (!OnUpdateClip.IsBound() || !OnUpdateAmmo.IsBound()) return;
	OnUpdateClip.Execute(InWeaponType, InClip);
	OnUpdateAmmo.Execute(InWeaponType, InAmmo);
}

void ACPP_Character::SpawnCombatWeapon(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType)
{
	SpawnCombatWeapon(WeaponInstance, WeaponType);
}

void ACPP_Character::Server_SpawnCombatWeapon_Implementation(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType)
{
	const FAttachmentTransformRules AttachmentTransformRules{ EAttachmentRule::SnapToTarget, false };

	CombatWeapon = GetWorld()->SpawnActor<ACPP_Weapon_Base>(WeaponInstance);
	if (CombatWeapon == nullptr) return;

	CombatWeapon->SetWeaponType(WeaponType);

	switch (WeaponType)
	{
	case EWeaponType::Rifle:
	{
		CombatWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, RifleHandSocket);
		break;
	}
	case EWeaponType::Pistol:
	{
		CombatWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, PistolHandSocket);
		break;
	}
	}
}

void ACPP_Character::SpawnEquippedWeapon(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType)
{
	const FAttachmentTransformRules AttachmentTransformRules{ EAttachmentRule::SnapToTarget, false };

	EquippedWeapon = GetWorld()->SpawnActor<ACPP_Weapon>(WeaponInstance);
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->SetWeaponType(WeaponType);

	switch (WeaponType)
	{
	case EWeaponType::Rifle:
	{
		EquippedWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, RifleBackSocket);
		break;
	}
	case EWeaponType::Pistol:
	{
		EquippedWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, PistolBackSocket);
		break;
	}
	}
}

void ACPP_Character::Server_SpawnEquippedWeapon_Implementation(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType)
{
	SpawnEquippedWeapon(WeaponInstance, WeaponType);
}

const bool ACPP_Character::IsAlreadyEquipped(const EWeaponType WeaponType) const
{
	if (CombatWeapon->GetWeaponType() == WeaponType)
	{
		AddAmmo(WeaponType);
		return true;
	}
	return false;
}

void ACPP_Character::ReattachWeapon(const FName& BackSocket, const FName& HandSocket)
{
	const FAttachmentTransformRules AttachmentTransformRules{ EAttachmentRule::SnapToTarget, false };
	ACPP_Weapon* TemporaryWeapon{ CombatWeapon };

	CombatWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, BackSocket);
	EquippedWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, HandSocket);

	CombatWeapon = EquippedWeapon;
	EquippedWeapon = TemporaryWeapon;
}

void ACPP_Character::ChangeArmorIndicator(const float DeltaArmor)
{
	Armor = FMath::Clamp(Armor + DeltaArmor, 0.f, MaxArmor);

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		OnUpdateArmor.Execute(Armor);
	}
}

void ACPP_Character::ChangeHealthIndicator(const float DeltaHealth, FKillInfo& KillInfo)
{
	if (bIsDeath) return;

	Health = FMath::Clamp(Health + DeltaHealth, 0.f, MaxHealth);

	if (Health == 0.f)
	{
		Death(KillInfo);
		return;
	}

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		OnUpdateHealth.Execute(Health);
	}
}

void ACPP_Character::RemoveMappingContext() const
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(WeaponMappingContext);
			Subsystem->RemoveMappingContext(CharacterMappingContext);
		}
	}
}

void ACPP_Character::Death(FKillInfo& KillInfo)
{
	GetMesh()->SetSimulatePhysics(true);

	FTimerHandle FireRateHandle;
	GetWorld()->GetTimerManager().SetTimer(FireRateHandle, [this] { RespawnPlayer(); }, 5.f, false);

	ACPP_PlayerController* PlayerController{ GetController<ACPP_PlayerController>() };
	if (PlayerController == nullptr) return;

	KillPlayer(KillInfo);

	PlayerController->Client_RemovePlayerNameWidget();
	PlayerController->Client_UpdateInterface(false);

	RemoveMappingContext();

	bIsDeath = true;
}

// Called to bind functionality to input
void ACPP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPP_Character_Base::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPP_Character_Base::Look);

		//Weapon
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACPP_Character_Base::AimStart);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACPP_Character_Base::AimEnd);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACPP_Character_Base::Fire);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ACPP_Character_Base::Reload);

		EnhancedInputComponent->BindAction(ScrollAction, ETriggerEvent::Started, this, &ACPP_Character_Base::ScrollWeapon);
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, CharacterMappingContextPriority);
		}
	}
}

void ACPP_Character::PickupWeapon(const FName& WeaponName)
{
	const UCPP_GameInstance* GameInstance{ GetGameInstance<UCPP_GameInstance>() };
	if (GameInstance == nullptr) return;

	const FWeaponTable* WeaponInfo{ GameInstance->GetWeaponByRowName(WeaponName) };
	if (WeaponInfo == nullptr) return;

	switch (EquipState)
	{
	case EEquipState::None:
	{
		SpawnCombatWeapon(WeaponInfo->WeaponInstance, WeaponInfo->WeaponType);

		bIsEquipRifle = WeaponInfo->WeaponType == EWeaponType::Rifle;
		bIsEquipWeapon = true;

		AddWeaponMappingContext();
		TryUpdateWeaponWidgets();
		PlayTimeline(true);

		break;
	}
	case EEquipState::Equipped:
	{
		if (!IsAlreadyEquipped(WeaponInfo->WeaponType))
		{
			SpawnEquippedWeapon(WeaponInfo->WeaponInstance, WeaponInfo->WeaponType);
			TryUpdateAmmoClipWidgets(EquippedWeapon->GetWeaponType(), EquippedWeapon->GetCurrentClip(), EquippedWeapon->GetCurrentAmmo());

			EquipState = EEquipState::FullEquipped;
			break;
		}
	}
	case EEquipState::FullEquipped:
	{
		AddAmmo(WeaponInfo->WeaponType);
		break;
	}
	}
}

void ACPP_Character::AddAmmo(const EWeaponType WeaponType) const
{
	if (CombatWeapon->GetWeaponType() == WeaponType)
	{
		CombatWeapon->AddAmmoToWeapon();

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			TryUpdateAmmoClipWidgets(WeaponType, CombatWeapon->GetCurrentClip(), CombatWeapon->GetCurrentAmmo());
		}
	}
	else
	{
		EquippedWeapon->AddAmmoToWeapon();

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			TryUpdateAmmoClipWidgets(WeaponType, EquippedWeapon->GetCurrentClip(), EquippedWeapon->GetCurrentAmmo());
		}
	}
}

void ACPP_Character::AddHealth(const float AddHealth)
{
	FKillInfo KillInfo{};
	ChangeHealthIndicator(AddHealth, KillInfo);
}

void ACPP_Character::AddArmor(const float AddArmor)
{
	ChangeArmorIndicator(AddArmor);
}

void ACPP_Character::ChangeWeapon()
{
	switch (CombatWeapon->GetWeaponType())
	{
	case EWeaponType::Rifle:
	{
		ReattachWeapon(RifleBackSocket, PistolHandSocket);
		break;
	}
	case EWeaponType::Pistol:
	{
		ReattachWeapon(RifleBackSocket, PistolHandSocket);
		break;
	}
	}
}

void ACPP_Character::FinishChangeWeapon()
{
	bIsScrollWeapon = false;
	bIsEquipRifle = CombatWeapon->GetWeaponType() == EWeaponType::Rifle;

	if (GetLocalRole() == ROLE_AutonomousProxy)
		TryUpdateWeaponWidgets();
}

void ACPP_Character::FinishReloadWeapon()
{
	bIsReload = false;

	if (GetLocalRole() == ROLE_AutonomousProxy)
		TryUpdateAmmoClipWidgets(CombatWeapon->GetWeaponType(), CombatWeapon->GetCurrentClip(), CombatWeapon->GetCurrentAmmo());
}

void ACPP_Character::FinishReceiveDamage()
{
	bIsReceiveDamage = false;
}

bool ACPP_Character::ApplyDamage(const float Damage, FKillInfo& KillInfo)
{
	const float FinalDamage{ KillInfo.bIsHeadshot ? Damage * 2.f : Damage };

	if (Armor >= FMath::Abs(-FinalDamage))
	{
		ChangeArmorIndicator(-FinalDamage);
	}
	else
	{
		bIsReceiveDamage = true;
		ChangeArmorIndicator(-Armor);
		ChangeHealthIndicator(-FinalDamage + Armor, KillInfo);
	}
	return false;
}

void ACPP_Character::KillPlayer(FKillInfo& KillInfo) const
{
	ACPP_PlayerController* PlayerController{ GetController<ACPP_PlayerController>() };
	if (PlayerController == nullptr) return;

	PlayerController->OnKillPlayer(KillInfo);
}

void ACPP_Character::Server_DestroyActor_Implementation()
{
	Multicast_DestroyActor();
}

void ACPP_Character::Multicast_DestroyActor_Implementation()
{
	Destroy();
}

void ACPP_Character::RespawnPlayer()
{
	ACPP_PlayerController* PlayerController{ GetController<ACPP_PlayerController>() };
	if (PlayerController == nullptr) return;

	UpdateEssentials();

	TryUpdateAmmoClipWidgets(EWeaponType::Pistol, 0, 0);
	TryUpdateAmmoClipWidgets(EWeaponType::Rifle, 0, 0);

	PlayerController->Server_RespawnPlayer(false);
}

void ACPP_Character::Server_DestroyWeapons_Implementation()
{
	Multicast_DestroyWeapons();
}

void ACPP_Character::Multicast_DestroyWeapons_Implementation()
{
	if (CombatWeapon)
	{
		CombatWeapon->Destroy();
		CombatWeapon = nullptr;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}
}

void ACPP_Character::UpdateEssentials()
{
	Server_DestroyWeapons();

	bIsEquipWeapon = false;
	bIsEquipRifle = false;
	bIsAimWeapon = false;
	bIsScrollWeapon = false;

	bIsReload = false;
	bIsReceiveDamage = false;
	bIsFireReady = true;

	DirectionMoveX = 0.f;
	DirectionMoveY = 0.f;

	BlendAlphaWeapon = 0.f;
}
