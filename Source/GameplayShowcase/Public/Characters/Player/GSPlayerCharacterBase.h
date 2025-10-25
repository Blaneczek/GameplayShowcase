// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GSPlayerCharacterBase.generated.h"

class UAttributeSet;
class UGSAttributeSetPlayer;
class UGameplayEffect;
class UGSAbilitySystemComponent;
class UGSSpringArmComponent;
class UGSCameraComponent;

UCLASS()
class GAMEPLAYSHOWCASE_API AGSPlayerCharacterBase : public ACharacter, public IAbilitySystemInterface
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

	/** Order is important! Character -> Primary -> Vital */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultAttributesEffectClasses;
	
private:
	void InitializeAttributes();
	void AddAbilities();
};
