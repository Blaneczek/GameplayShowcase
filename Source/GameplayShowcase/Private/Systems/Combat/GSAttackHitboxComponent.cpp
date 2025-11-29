// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/GSAttackHitboxComponent.h"


UGSAttackHitboxComponent::UGSAttackHitboxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

UGSAttackHitboxComponent* UGSAttackHitboxComponent::FindAttackHitboxComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSAttackHitboxComponent>() : nullptr;
}


void UGSAttackHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

}
