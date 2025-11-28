// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/GSCombatComponent.h"


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
}

