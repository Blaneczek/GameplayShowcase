// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/AbilitySystem/Abilities/Player/GSAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "Systems/Combat/GSCombatComponent.h"
#include "Systems/Combat/Data/GSComboInfo.h"
#include "Systems/Inventory/GSEquipmentComponent.h"

UGSAttack::UGSAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGSAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ComboInfo || !ComboInfo->HasValidCombo())
	{
		return false;
	}
	return true;
}

void UGSAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	CombatComponent = UGSCombatComponent::FindCombatComponent(ActorInfo->AvatarActor.Get());
	EquipmentComponent = UGSEquipmentComponent::FindEquipmentComponent(ActorInfo->AvatarActor.Get());
	UGSCombatComponent* CombatComp = CombatComponent.Get();
	UGSEquipmentComponent* EquipComp = EquipmentComponent.Get();
	if (!CombatComp || !EquipComp )
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bool bFound = false;
	float AttackSpeed = UAbilitySystemBlueprintLibrary::GetFloatAttribute(ActorInfo->AvatarActor.Get(), UGSAttributeSetBase::GetAttackSpeedAttribute(), bFound);
	if (!bFound)
	{
		AttackSpeed = 100.f;
		return;
	}
	SetAnimPlayRate(AttackSpeed);

	ActorInfo->AbilitySystemComponent.Get()->AddLooseGameplayTag(GSGameplayTags::Statuses::Movement_Forbidden.GetTag());
	
	CombatComp->SetIsAttacking(true);
	EquipComp->OnWeaponUnequippedDelegate.AddUObject(this, &UGSAttack::OnWeaponUnequipped);

	CurrentSectionIndex = 0;
	bContinueCombo = false;
	bInComboWindow = false;

	CreateEventTasks();
	
	if (!StartNextSection())
	{
		FinishAbility();
	}
}

void UGSAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UGSEquipmentComponent* EquipComp = EquipmentComponent.Get())
	{
		EquipComp->OnWeaponUnequippedDelegate.RemoveAll(this);
	}
	if (UGSCombatComponent* CombatComp = CombatComponent.Get())
	{
		CombatComp->SetIsAttacking(false);
	}

	CleanupMontageTask();
	CleanupEventTasks();

	bContinueCombo = false;
	bInComboWindow = false;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGSAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
 	if (IsActive() && bInComboWindow)
	{
		bContinueCombo = true;
	}
}

bool UGSAttack::StartNextSection()
{
	if (!CheckCost(CurrentSpecHandle, CurrentActorInfo))
	{
		return false;
	}
	
	if (!ComboInfo || !ComboInfo->HasValidCombo())
	{
		return false;
	}

	const int32 LastIndex = ComboInfo->GetLastComboIndex();
	if (CurrentSectionIndex > LastIndex)
	{
		CurrentSectionIndex = 0;  
	}
	
	CleanupMontageTask();
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ComboInfo->GetMontage(),
		AnimPlayRate,
		ComboInfo->GetComboSectionNameOnIndex(CurrentSectionIndex),
		true);

	if (!MontageTask)
	{
		return false;
	}
 	MontageTask->OnCompleted.AddDynamic(this, &UGSAttack::OnSectionFinished);
	MontageTask->OnCancelled.AddDynamic(this, &UGSAttack::OnSectionFinished);

	MontageTask->ReadyForActivation();

	++CurrentSectionIndex;
	CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	
	return true;
}

void UGSAttack::CleanupMontageTask()
{
	if (MontageTask)
	{
		MontageTask->OnCompleted.RemoveDynamic(this, &UGSAttack::OnSectionFinished);
		MontageTask->OnCancelled.RemoveDynamic(this, &UGSAttack::OnSectionFinished);
		MontageTask->EndTask();
		MontageTask = nullptr;
	}
}

void UGSAttack::CleanupEventTasks()
{
	if (WindowStartTask)
	{
		WindowStartTask->EventReceived.RemoveDynamic(this, &UGSAttack::OnComboWindowStart);
		WindowStartTask->EndTask();
		WindowStartTask = nullptr;
	}
	if (WindowEndTask)
	{
		WindowEndTask->EventReceived.RemoveDynamic(this, &UGSAttack::OnComboWindowEnd);
		WindowEndTask->EndTask();
		WindowEndTask= nullptr;
	}
	if (TraceStartTask)
	{
		TraceStartTask->EventReceived.RemoveDynamic(this, &UGSAttack::OnAttackTraceStart);
		TraceStartTask->EndTask();
		TraceStartTask = nullptr;
	}
	if (TraceEndTask)
	{
		TraceEndTask->EventReceived.RemoveDynamic(this, &UGSAttack::OnAttackTraceEnd);
		TraceEndTask->EndTask();
		TraceEndTask = nullptr;
	}
	if (EnableMovementTask)
	{
		EnableMovementTask->EventReceived.RemoveDynamic(this, &UGSAttack::OnEnableMovement);
		EnableMovementTask->EndTask();
		EnableMovementTask = nullptr;
	}
}

void UGSAttack::OnSectionFinished()
{
	if (bContinueCombo)
	{
		bContinueCombo = false;
		if (!StartNextSection())
		{
			FinishAbility();
		}
	}
	else
	{
		FinishAbility();
	}
}

void UGSAttack::OnComboWindowStart(FGameplayEventData Payload)
{
	bInComboWindow = true;
}

void UGSAttack::OnComboWindowEnd(FGameplayEventData Payload)
{
	bInComboWindow = false;
	if (!bContinueCombo)
	{
		return;		
	}
	MontageTask ? MontageTask->ExternalCancel() : FinishAbility();
}

void UGSAttack::OnAttackTraceStart(FGameplayEventData Payload)
{
	if (UGSCombatComponent* CombatComp = CombatComponent.Get())
	{
		CombatComp->StartComboAttackTrace(ComboInfo->GetAttackRange(), ComboInfo->GetTraceShape());
	}
}

void UGSAttack::OnAttackTraceEnd(FGameplayEventData Payload)
{
	if (UGSCombatComponent* CombatComp = CombatComponent.Get())
	{
		CombatComp->StopComboAttackTrace();
	}
}

void UGSAttack::OnEnableMovement(FGameplayEventData Payload)
{
	if (CurrentActorInfo)
	{
		CurrentActorInfo->AbilitySystemComponent.Get()->RemoveLooseGameplayTag(GSGameplayTags::Statuses::Movement_Forbidden.GetTag());
	}	
}

void UGSAttack::OnWeaponUnequipped()
{
	if (IsActive())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	}
}

void UGSAttack::FinishAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGSAttack::CreateEventTasks()
{
	WindowStartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GSGameplayTags::Events::Montage_Combo_Window_Start.GetTag());
	if (WindowStartTask)
	{
		WindowStartTask->EventReceived.AddDynamic(this, &UGSAttack::OnComboWindowStart);
		WindowStartTask->Activate();
	}

	WindowEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GSGameplayTags::Events::Montage_Combo_Window_End.GetTag());
	if (WindowEndTask)
	{
		WindowEndTask->EventReceived.AddDynamic(this, &UGSAttack::OnComboWindowEnd);
		WindowEndTask->Activate();
	}

	TraceStartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GSGameplayTags::Events::Montage_Combo_Trace_Start.GetTag());
	if (TraceStartTask)
	{
		TraceStartTask->EventReceived.AddDynamic(this, &UGSAttack::OnAttackTraceStart);
		TraceStartTask->Activate();
	}

	TraceEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GSGameplayTags::Events::Montage_Combo_Trace_End.GetTag());
	if (TraceEndTask)
	{
		TraceEndTask->EventReceived.AddDynamic(this, &UGSAttack::OnAttackTraceEnd);
		TraceEndTask->Activate();
	}

	EnableMovementTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		GSGameplayTags::Events::Montage_Combo_EnableMovement.GetTag());
	if (EnableMovementTask )
	{
		EnableMovementTask ->EventReceived.AddDynamic(this, &UGSAttack::OnEnableMovement);
		EnableMovementTask ->Activate();
	}
}

void UGSAttack::SetAnimPlayRate(float AttackSpeed)
{
	AnimPlayRate = AttackSpeed / 100.f;
}
