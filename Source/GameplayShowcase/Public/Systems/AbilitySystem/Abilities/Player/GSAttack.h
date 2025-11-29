// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/AbilitySystem/Abilities/GSGameplayAbility.h"
#include "GSAttack.generated.h"

class UGSEquipmentComponent;
class UGSCombatComponent;
class UGSComboInfo;
/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSAttack : public UGSGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void InputPressed(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	bool bContinueCombo = false;
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, 
		bool bWasCancelled) override;
	
	UPROPERTY(EditAnywhere, Category="GS|Combat")
	TObjectPtr<UGSComboInfo> ComboActions = nullptr;

	TWeakObjectPtr<UGSCombatComponent> CombatComponent;
	TWeakObjectPtr<UGSEquipmentComponent> EquipmentComponent;

private:
	void OnWeaponUnequipped();

	
};
