// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CPP_MainWeapon.h"
#include "CPP_Character.generated.h"

class ACPP_MainWeapon;
struct FKillInfo;

DECLARE_DELEGATE(FOnFireWeapon);
DECLARE_DELEGATE_OneParam(FOnUpdateHealth, float /*Health*/)
DECLARE_DELEGATE_OneParam(FOnUpdateArmor, float /*Armor*/)
DECLARE_DELEGATE_OneParam(FOnEquipWeapon, EWeaponType /*WeaponType*/)
DECLARE_DELEGATE_TwoParams(FOnUpdateClip, EWeaponType /*WeaponType*/, uint8 /*Clip num*/)
DECLARE_DELEGATE_TwoParams(FOnUpdateAmmo, EWeaponType /*WeaponType*/, uint8 /*Ammo num*/)

UENUM()
enum class EEquipState : uint8
{
	None UMETA(DisplayName = "Rifle"),
	Equipped UMETA(DisplayName = "Equipped"),
	FullEquipped UMETA(DisplayName = "FullEquipped"),
};

UCLASS()
class TESTTPS_API ACPP_Character : public ACharacter
{
	GENERATED_BODY()

#pragma region OverrideFunctions

public:
	ACPP_Character();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region Functions
private:
#pragma region Inputs
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void AimStart(const FInputActionValue& Value);
	void AimEnd(const FInputActionValue& Value);

	void Fire(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);
	void ScrollWeapon(const FInputActionValue& Value);
	void AddWeaponMappingContext() const;
#pragma endregion

#pragma region InputHelperFunction
	// Move
	UFUNCTION(Server, Reliable)
	void Server_SetDirectionX(const float XValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDirectionX(const float XValue);

	UFUNCTION(Server, Reliable)
	void Server_SetDirectionY(const float YValue);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetDirectionY(const float YValue);

	// Look
	void CalculatePitchRotator();

	UFUNCTION(Server, Reliable)
	void Server_SetCalculatedPitchRotator(const FRotator& InRotator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCalculatedPitchRotator(const FRotator& InRotator);

	// Aim
	void SetAimMode(const bool Value);

	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Aim(const bool Value);

	// Fire
	UFUNCTION(Server, Reliable)
	void Server_Fire(const FVector& ProjectileDirection, const FString& PlayerName);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire(const FVector& ProjectileDirection, const FString& PlayerName);

	void SetFireRate();

	// Reload
	UFUNCTION(Server, Reliable)
	void Server_Reload(const bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reload(const bool Value);

	// Scroll
	const bool IsInvalidAction() const;

	UFUNCTION(Server, Reliable)
	void Server_Scroll(const bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Scroll(const bool Value);
#pragma endregion

	void PlayTimeline(bool bIsFromStart) const;

	UFUNCTION()
	void TimelineCallback(float Value);

	UFUNCTION()
	void TimelineEventFinish();

	void TryUpdateWeaponWidgets() const;
	void TryUpdateAmmoClipWidgets(const EWeaponType InWeaponType, const uint8 InClip, const uint8 InAmmo) const;
	
	void SpawnCombatWeapon(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType);

	UFUNCTION(Server, Reliable)
	void Server_SpawnCombatWeapon(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType);
	void SpawnEquippedWeapon(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType);

	UFUNCTION(Server, Reliable)
	void Server_SpawnEquippedWeapon(TSubclassOf<ACPP_Weapon> WeaponInstance, const EWeaponType WeaponType);

	const bool IsAlreadyEquipped(const EWeaponType WeaponType) const;

	void ReattachWeapon(const FName& BackSocket, const FName& HandSocket);

	void ChangeArmorIndicator(const float DeltaArmor);
	void ChangeHealthIndicator(const float DeltaHealth, FKillInfo& KillInfo);

	void RemoveMappingContext() const;
	void Death(FKillInfo& KillInfo);

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE FRotator GetCalculatedPitchRotator() const { return CalculatedPitchRotator; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsEquipWeapon() const { return bIsEquipWeapon; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsLootRifle() const { return bIsEquipRifle; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsAimWeapon() const { return bIsAimWeapon; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsScrollWeapon() const { return bIsScrollWeapon; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetBlendAlphaWeapon() const { return BlendAlphaWeapon; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetDirectionMoveX() const { return DirectionMoveX; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetDirectionMoveY() const { return DirectionMoveY; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsReload() const { return bIsReload; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetIsReceiveDamage() const { return bIsReceiveDamage; }

	FORCEINLINE class UWidgetComponent* GetPlayerNameWidgetComponent() const { return PlayerNameWidgetComponent; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMaxArmor() const { return MaxArmor; }

	void PickupWeapon(const FName& WeaponName);
	void AddAmmo(const EWeaponType WeaponType) const;
	void AddHealth(const float AddHealth);
	void AddArmor(const float AddArmor);

	void ChangeWeapon();
	void FinishChangeWeapon();
	void FinishReloadWeapon();
	void FinishReceiveDamage();

	bool ApplyDamage(const float Damage, FKillInfo& KillInfo);
	void KillPlayer(FKillInfo& KillInfo) const;

	UFUNCTION(Server, Reliable)
	void Server_DestroyActor();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroyActor();

	void RespawnPlayer();

	UFUNCTION(Server, Reliable)
	void Server_DestroyWeapons();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroyWeapons();

	void UpdateEssentials();

#pragma endregion

#pragma region Variables
	FOnFireWeapon OnFireWeapon;

	FOnUpdateHealth OnUpdateHealth;
	FOnUpdateArmor OnUpdateArmor;

	FOnEquipWeapon OnEquipWeapon;
	FOnUpdateClip OnUpdateClip;
	FOnUpdateAmmo OnUpdateAmmo;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	class USpringArmComponent* CameraBoom{ nullptr };

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	class UCameraComponent* FollowCamera{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Camera)
	UWidgetComponent* PlayerNameWidgetComponent{ nullptr };

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	class UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputMappingContext* WeaponMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	int32 CharacterMappingContextPriority{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	int32 WeaponMappingContextPriority{ 1 };

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputAction* ReloadAction;

	/** Mouse Wheel */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Input)
	UInputAction* ScrollAction;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	FName RifleHandSocket{ "None" };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	FName RifleBackSocket{ "None" };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	FName PistolHandSocket{ "None" };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Weapon)
	FName PistolBackSocket{ "None" };

	UPROPERTY()
	ACPP_Weapon* CombatWeapon{ nullptr };

	UPROPERTY()
	ACPP_Weapon* EquippedWeapon{ nullptr };

	UPROPERTY(Replicated)
	FRotator CalculatedPitchRotator{};

	UPROPERTY(Replicated)
	class UTimelineComponent* CameraTimeline{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Timeline)
	UCurveFloat* FloatCurve{ nullptr };

	EEquipState EquipState{ EEquipState::None };

	float Health{ 100.f };
	float Armor{ 0.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
	float MaxHealth{ 100.f };
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = CharacterStats)
	float MaxArmor{ 100.f };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Effects)
	UParticleSystem* HealthDamageParticleFX{ nullptr };

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = Effects)
	UParticleSystem* ArmorDamageParticleFX{ nullptr };

	bool bIsEquipWeapon{ false };
	bool bIsEquipRifle{ false };
	bool bIsAimWeapon{ false };
	bool bIsScrollWeapon{ false };

	bool bIsReload{ false };
	bool bIsReceiveDamage{ false };
	bool bIsFireReady{ true };

	float DirectionMoveX{ 0.f };
	float DirectionMoveY{ 0.f };

	bool bIsDeath{ false };

	UPROPERTY(Replicated)
	float BlendAlphaWeapon{ 0.f };

#pragma endregion
};
