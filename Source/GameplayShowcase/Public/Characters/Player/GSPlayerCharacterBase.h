// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Interfaces/GSAbilityCharacterHelper.h"
#include "GameFramework/Character.h"
#include "GSPlayerCharacterBase.generated.h"

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
	UGSSpringArmComponent* GetCameraArm() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UAttributeSet* GetAttributeSet() const;

	virtual void SetMovementSpeed(bool bSprint = true, float NewSpeed = 500.f) override;
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UGSCameraComponent> Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UGSSpringArmComponent> CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TObjectPtr<UGSAttributeSetPlayer> AttributeSet;

	/*** Gameplay Effects ***/
	
	/** Order is important! Character -> Primary -> Vital */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultAttributesEffectClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TSubclassOf<UGameplayEffect> STRegenEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
    float StaminaRegenDelay = 1.f;
	/************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	
	
private:
	void InitializeAttributes();
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void ApplySimpleGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level);
	
	void CheckIfCharacterIsMoving();

	void OnStaminaConsumingTagChanged(const FGameplayTag Tag, int32 NewCount);
	
private:
	float DefaultMovementSpeed;

	FTimerHandle IsMovingTimerHandle;
	FTimerHandle StaminaRegenTimerHandle;

	
};
