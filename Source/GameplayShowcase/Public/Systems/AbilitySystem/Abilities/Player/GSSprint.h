// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/AbilitySystem/Abilities/GSGameplayAbility.h"
#include "GSSprint.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSSprint : public UGSGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SprintSpeedMovement = 700.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinRequiredStamina = 10.f;
};
