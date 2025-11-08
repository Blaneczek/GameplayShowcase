// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "Systems/Inventory/Items/GSItemComponent.h"

void FItemDefinition::SpawnWorldActor(const UObject* WorldContextObject, const FTransform& SpawnTransform)
{
	if (!WorldActorClass || !WorldContextObject)
	{
		return;
	}

	if (const AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(WorldActorClass, SpawnTransform))
	{
		UGSItemComponent* ItemComp = UGSItemComponent::FindItemComponent(SpawnedActor);
		checkf(ItemComp, TEXT("FItemDefinition::SpawnWorldActor | Item component not found in spawned actor"));

		//TODO: ItemComp->InitItemDefinition(*this);
	}
}

void FItemDefinition::ClearFragments()
{
	Fragments.Empty();
}
