// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"

#include "GameplayEffectExtension.h"
#include "GSGameplayTags.h"

UGSAttributeSetPlayer::UGSAttributeSetPlayer()
{
	/* Vital Attributes */
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_HP.GetTag(), GetHPAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_PE.GetTag(), GetPEAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_ST.GetTag(), GetSTAttribute);
	
	/* Character Attributes */
	TagsToAttributes.Add(GSGameplayTags::Attribute_Character_VIT.GetTag(), GetVITAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Character_INT.GetTag(), GetINTAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Character_STR.GetTag(), GetSTRAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Character_DEX.GetTag(), GetDEXAttribute);

	/* Primary Attributes */
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_MaxHP.GetTag(), GetMaxHPAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_HPRegen.GetTag(), GetHPRegenAttribute);	
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_MaxPE.GetTag(), GetMaxPEAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_PERegen.GetTag(), GetPERegenAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_MaxST.GetTag(), GetMaxSTAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_STRegen.GetTag(), GetSTRegenAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_AttackDamage.GetTag(), GetAttackDamageAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_MagicDamage.GetTag(), GetMagicDamageAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_Defence.GetTag(), GetDefenceAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_MagicDefence.GetTag(), GetMagicDefenceAttribute);
	TagsToAttributes.Add(GSGameplayTags::Attribute_Primary_Evading.GetTag(), GetEvadingAttribute);
}

void UGSAttributeSetPlayer::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetSTAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxST());
	}
}

void UGSAttributeSetPlayer::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetSTAttribute())
	{
		SetST(FMath::Clamp(GetST(), 0.f, GetMaxST()));
		if (GetST() <= 0.f)
		{	
			const FGameplayTagContainer Container{GSGameplayTags::Ability_Requires_ST.GetTag()};
			Data.Target.CancelAbilities(&Container);
		}
	}
}
