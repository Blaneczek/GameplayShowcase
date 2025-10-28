// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "AbilitySystem/Abilities/Player/GSSprint.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"

bool UGSSprint::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                          FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}
	
	// Minimum Stamina to perform Ability
	return ActorInfo->AbilitySystemComponent->GetNumericAttribute(UGSAttributeSetPlayer::GetSTAttribute()) >= 1.f;
}
