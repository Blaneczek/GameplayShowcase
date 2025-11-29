// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/AbilitySystem/Abilities/Player/GSAttack.h"

#include "Systems/Combat/GSCombatComponent.h"
#include "Systems/Combat/Data/GSComboInfo.h"
#include "Systems/Inventory/GSEquipmentComponent.h"

bool UGSAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ComboActions || !ComboActions->HasValidCombo())
	{
		return false;
	}
	return true;
}

void UGSAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive())
	{
		bContinueCombo = true;
	}	
}

void UGSAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UGSCombatComponent* CombatComp = UGSCombatComponent::FindCombatComponent(ActorInfo->AvatarActor.Get());
	if (!CombatComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	CombatComponent = CombatComp;

	UGSEquipmentComponent* EquipmentComp = UGSEquipmentComponent::FindEquipmentComponent(ActorInfo->AvatarActor.Get());
	if (!EquipmentComp)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	EquipmentComponent = EquipmentComp;
	EquipmentComp->OnWeaponUnequippedDelegate.AddUObject(this, &UGSAttack::OnWeaponUnequipped);

	if (bContinueCombo)
	{
		bContinueCombo = false;
	}
	else
	{
		CombatComp->ResetComboAttack();
	}
	CombatComp->BeginComboAttack(ComboActions->GetLastIndex());
}

void UGSAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UGSEquipmentComponent* EquipmentComp = EquipmentComponent.Get())
	{
		EquipmentComp->OnWeaponUnequippedDelegate.RemoveAll(this);
	}
}

void UGSAttack::OnWeaponUnequipped()
{
	if (IsActive())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	}
}
