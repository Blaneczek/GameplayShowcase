// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GSAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGSAbilitySystemComponent();

	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void AddCharacterAbility(TSubclassOf<UGameplayAbility> AbilityClass);
	void RemoveCharacterAbility(TSubclassOf<UGameplayAbility> AbilityClass);

	/* Input */
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);


};
