// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"

UGSAttributeSetBase::UGSAttributeSetBase()
{
}

void UGSAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHP());
	}
	else if (Attribute == GetPEAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPE());
	}
}

void UGSAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(), 0.f, GetMaxHP()));
		if (GetHP() <= 0.f)
		{	
			// Handle death
		}
		else if (GetHP() >= GetMaxHP())
		{
			// Remove regen effect if HP is full
			Data.Target.RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(GSGameplayTags::Status_HP_Regen.GetTag()));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetPEAttribute())
	{
		SetPE(FMath::Clamp(GetPE(), 0.f, GetMaxPE()));
		if (GetPE() >= GetMaxPE())
		{
			// Remove regen effect if PE is full
			Data.Target.RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(GSGameplayTags::Status_PE_Regen.GetTag()));
		}
	}
}
