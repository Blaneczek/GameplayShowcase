// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/GSCombatComponent.h"

#include "Systems/Combat/GSAttackHitboxComponent.h"


UGSCombatComponent::UGSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UGSCombatComponent* UGSCombatComponent::FindCombatComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSCombatComponent>() : nullptr;
}

void UGSCombatComponent::BeginComboAttack(int32 LastComboIndex)
{
	if (CurrentComboIndex > LastComboIndex)
	{
		ResetComboAttack();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("combo index: %d"), CurrentComboIndex)		
	CurrentComboIndex++;;
}

void UGSCombatComponent::ActivateHitbox()
{
}

void UGSCombatComponent::DeactivateHitbox()
{
}

void UGSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner())
	{
		return;
	}
	
	if (UGSAttackHitboxComponent* HitboxComp = UGSAttackHitboxComponent::FindAttackHitboxComponent(GetOwner()))
	{
		// todo: bind delegates
		HitboxComponent = HitboxComp;
	}
}

void UGSCombatComponent::ResetComboAttack()
{
	CurrentComboIndex = 0;
}

