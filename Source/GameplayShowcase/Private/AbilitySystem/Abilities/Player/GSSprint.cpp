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

	// Minimum Stamina to perform Ability and Actor has to be moving
	return ActorInfo->AbilitySystemComponent->GetNumericAttribute(UGSAttributeSetPlayer::GetSTAttribute()) >= MinRequiredStamina
		   && ActorInfo->AvatarActor->GetVelocity().SquaredLength() >= 1.f;
}
