// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSAttributeSetBase.h"
#include "GSAttributeSetPlayer.generated.h"

using FAttributePair = TPair<FGameplayAttribute(*)(), FGameplayAttribute(*)()>;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSAttributeSetPlayer : public UGSAttributeSetBase
{
	GENERATED_BODY()
	
public:
	UGSAttributeSetPlayer();

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	TMap<FGameplayTag, FGameplayAttribute(*)()> TagsToSingleAttributes;
	TMap<FGameplayTag, FAttributePair> TagsToPairAttributes;

	/* VITAL ATTRIBUTES */
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData ST;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, ST);
	
	/* CHARACTER ATTRIBUTES */
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData VIT;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, VIT);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData INT;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, INT);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData STR;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, STR);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData DEX;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, DEX);

	/* PRIMARY ATTRIBUTES */
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxST;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, MaxST);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData STRegen;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, STRegen);

	/* OTHERS ATTRIBUTES */
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS_BASIC(UGSAttributeSetPlayer, XP);
};
