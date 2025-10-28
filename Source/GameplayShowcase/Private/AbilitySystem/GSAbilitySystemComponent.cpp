// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "AbilitySystem/GSAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/GSGameplayAbility.h"

UGSAbilitySystemComponent::UGSAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UGSAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// For proper working of WaitInputRelease gameplay ability task
	if(Spec.IsActive())
	{
		TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
		const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
		FPredictionKey OriginalPredictionKey = ActivationInfo.GetActivationPredictionKey();
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
	}
}

void UGSAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const auto& AbilityClass : Abilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
			if (const UGSGameplayAbility* GSAbility = Cast<UGSGameplayAbility>(AbilitySpec.Ability))
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(GSAbility->DefaultInputTag);
				GiveAbility(AbilitySpec);
			}
		}
	}
}

void UGSAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UGSAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}



