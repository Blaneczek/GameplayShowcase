// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "AbilitySystem/AttributeSets/GSAttributeSetBase.h"
#include "GameplayEffectExtension.h"

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
		SetHP(GetHP());
	}
	else if (Data.EvaluatedData.Attribute == GetPEAttribute())
	{
		SetPE(GetPE());
	}
}
