// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GSAttributeSetBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGSAttributeSetBase();

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
public:
	/* VITAL ATTRIBUTES */
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
    FGameplayAttributeData HP;
    ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, HP);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData PE;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, PE);
	
	/* PRIMARY ATTRIBUTES */
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxHP;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, MaxHP);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData HPRegen;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, HPRegen);
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxPE;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, MaxPE);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData PERegen;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, PERegen);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, AttackDamage);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MagicDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, MagicDamage);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Defence;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, Defence);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MagicDefence;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, MagicDefence);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Evading;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetBase, Evading);
};
