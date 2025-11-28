// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Systems/Combat/GSDamageable.h"
#include "GSMonsterCharacterBase.generated.h"

class UAttributeSet;
class UGSAttributeSetBase;
class UGSAbilitySystemComponent;

UCLASS()
class GAMEPLAYSHOWCASE_API AGSMonsterCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGSDamageable
{
	GENERATED_BODY()

public:
	AGSMonsterCharacterBase();

	FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;	
	FORCEINLINE UAttributeSet* GetAttributeSet() const;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS|Attributes")
	TObjectPtr<UGSAttributeSetBase> AttributeSet;
	
};
