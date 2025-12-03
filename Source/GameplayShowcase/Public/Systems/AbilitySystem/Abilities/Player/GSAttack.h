// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/AbilitySystem/Abilities/GSGameplayAbility.h"
#include "GSAttack.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
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
	UGSAttack();
	
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

	
	void OnWeaponUnequipped();
	
	UPROPERTY(EditAnywhere, Category="GS|Combat")
	TObjectPtr<UGSComboInfo> ComboInfo = nullptr;

private:
	UFUNCTION()
    void OnSectionFinished();
    UFUNCTION()
    void OnComboWindowStart(FGameplayEventData Payload);
	UFUNCTION()
	void OnComboWindowEnd(FGameplayEventData Payload);
	UFUNCTION()
	void OnAttackTraceStart(FGameplayEventData Payload);
	UFUNCTION()
	void OnAttackTraceEnd(FGameplayEventData Payload);
	
	bool StartNextSection();
	void CleanupMontageTask();
	void CleanupEventTasks();
	void FinishAbility();
	void CreateEventTasks();
	
	TWeakObjectPtr<UGSCombatComponent> CombatComponent;
	TWeakObjectPtr<UGSEquipmentComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WindowStartTask= nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WindowEndTask= nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> TraceStartTask = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> TraceEndTask = nullptr;
	
	int32 CurrentSectionIndex = 0;
	bool bContinueCombo = false;
	bool bInComboWindow = false;
};
