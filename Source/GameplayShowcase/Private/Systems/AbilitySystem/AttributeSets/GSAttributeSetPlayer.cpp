// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"

#include "GameplayEffectExtension.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"

UGSAttributeSetPlayer::UGSAttributeSetPlayer()
{

	TagsToPairAttributes.Add(GSGameplayTags::Attributes::Primary_HP.GetTag(), {GetHPAttribute, GetMaxHPAttribute});
	TagsToPairAttributes.Add(GSGameplayTags::Attributes::Primary_PE.GetTag(), {GetPEAttribute, GetMaxPEAttribute});
	TagsToPairAttributes.Add(GSGameplayTags::Attributes::Primary_ST.GetTag(), {GetSTAttribute, GetMaxSTAttribute});

	TagsToPairAttributes.Add(GSGameplayTags::Attributes::Primary_AttackDamage.GetTag(), {GetAttackDamageMinAttribute, GetAttackDamageMaxAttribute});
	TagsToPairAttributes.Add(GSGameplayTags::Attributes::Primary_MagicDamage.GetTag(), {GetMagicDamageMinAttribute, GetMagicDamageMaxAttribute});
	
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Character_VIT.GetTag(), GetVITAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Character_INT.GetTag(), GetINTAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Character_STR.GetTag(), GetSTRAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Character_DEX.GetTag(), GetDEXAttribute);

	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_HPRegen.GetTag(), GetHPRegenAttribute);	
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_PERegen.GetTag(), GetPERegenAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_STRegen.GetTag(), GetSTRegenAttribute);
	
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_Defence.GetTag(), GetDefenceAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_MagicDefence.GetTag(), GetMagicDefenceAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_Evading.GetTag(), GetEvadingAttribute);
	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Primary_AttackSpeed.GetTag(), GetAttackSpeedAttribute);


	TagsToSingleAttributes.Add(GSGameplayTags::Attributes::Others_XP.GetTag(), GetXPAttribute);
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
		else if (GetST() >= GetMaxST())
		{
			// Remove regen effect if stamina is full
			Data.Target.RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(GSGameplayTags::Status_ST_Regen.GetTag()));
		}
	}
}
