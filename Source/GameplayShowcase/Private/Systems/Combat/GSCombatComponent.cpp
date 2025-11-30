// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/GSCombatComponent.h"
#include "Characters/Player/GSPlayerCharacterBase.h"

UGSCombatComponent::UGSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UGSCombatComponent* UGSCombatComponent::FindCombatComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSCombatComponent>() : nullptr;
}

void UGSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AGSPlayerCharacterBase* Owner = Cast<AGSPlayerCharacterBase>(GetOwner());
	if (!Owner)
	{
		return;
	}
	CachedOwner = Owner;
}

