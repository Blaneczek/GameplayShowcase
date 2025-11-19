// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Systems/AbilitySystem/Interfaces/GSAbilityCharacterHelper.h"
#include "GameFramework/Character.h"
#include "Systems/Leveling/GSLevelingComponent.h"
#include "GSPlayerCharacterBase.generated.h"

class USphereComponent;
class UGSEquipmentComponent;
class UGSInventoryComponent;
class UGSLevelingComponent;
struct FGameplayTag;
struct FOnAttributeChangeData;
class UGameplayAbility;
class UAttributeSet;
class UGSAttributeSetPlayer;
class UGameplayEffect;
class UGSAbilitySystemComponent;
class UGSSpringArmComponent;
class UGSCameraComponent;

UCLASS()
class GAMEPLAYSHOWCASE_API AGSPlayerCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGSAbilityCharacterHelper
{
	GENERATED_BODY()

public:
	AGSPlayerCharacterBase();

	void InitAbilityActorInfo();
	virtual void PossessedBy(AController* NewController) override;
	
	/* Getters */
	FORCEINLINE UGSSpringArmComponent* GetCameraArm() const { return CameraArm; };
	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;	
	FORCEINLINE UAttributeSet* GetAttributeSet() const;
	FORCEINLINE UGSLevelingComponent* GetLevelingComponent() const { return LevelingComponent; }
	FORCEINLINE UGSInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE int32 GetPlayerLevel() const { return LevelingComponent->GetLevel(); }
	
	virtual void SetMovementSpeed(bool bSprint = true, float NewSpeed = 500.f) override;

	void PickUpItem();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UGSCameraComponent> Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UGSSpringArmComponent> CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TObjectPtr<UGSAttributeSetPlayer> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TObjectPtr<UGSLevelingComponent> LevelingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TObjectPtr<UGSInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TObjectPtr<UGSEquipmentComponent> EquipmentComponent;

	/*** Gameplay Effects ***/
	
	/** Order is important! Character -> Primary -> Vital */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultAttributesEffectClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TSubclassOf<UGameplayEffect> STRegenEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TSubclassOf<UGameplayEffect> HPRegenEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TSubclassOf<UGameplayEffect> PERegenEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
    float RegenDelay = 1.f;
	
	/************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	
private:
	void InitializeAttributes();
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void ApplySimpleGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level);
	
	void CheckIfCharacterIsMoving();
	
	void OnSTConsumingTagChanged(const FGameplayTag Tag, int32 NewCount);
	void OnHPConsumingTagChanged(const FGameplayTag Tag, int32 NewCount);
	void OnPEConsumingTagChanged(const FGameplayTag Tag, int32 NewCount);
	void OnConsumingTagChanged(const FGameplayTag Tag, int32 NewCount, FTimerHandle& TimerHandle,
								TSubclassOf<UGameplayEffect> EffectClass, const FGameplayTag& RegenTag);
	

	float DefaultMovementSpeed;

	FTimerHandle IsMovingTimerHandle;
	FTimerHandle STRegenTimerHandle;
	FTimerHandle HPRegenTimerHandle;
	FTimerHandle PERegenTimerHandle;

};
