// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Loot/GSLootComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/Inventory/Items/GSItemComponent.h"
#include "Systems/Inventory/Items/GSWorldItemActor.h"


UGSLootComponent::UGSLootComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

UGSLootComponent* UGSLootComponent::FindLootComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSLootComponent>() : nullptr;
}

void UGSLootComponent::SpawnItems()
{
	UWorld* CachedWorld = GetWorld();
	if (!CachedWorld || !WorldItemClass)
	{
		return;
	}

	const FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, SpawnHeightOffset);
	for (const auto& Item : LootTable)
	{
		const float Roll = FMath::FRandRange(0.f, 100.f);
		if (Roll > Item.DropChance)
		{
			continue;
		}

		const int32 Quantity = FMath::RandRange(Item.QuantityRange.Min, Item.QuantityRange.Max);
		for (int32 i = 0; i < Quantity; i++)
		{
			SpawnWorldItem(CachedWorld, SpawnLocation, Item.Tag);
		}		
	}	
}

void UGSLootComponent::SpawnWorldItem(UWorld* World, const FVector& SpawnLocation, const FGameplayTag& ItemTag)
{
	FTransform SpawnTransform;
	const FVector RandomOffset = FMath::VRand() * 100.f;
	SpawnTransform.SetLocation(SpawnLocation + FVector(RandomOffset.X, RandomOffset.Y, 0.f));
	if (AGSWorldItemActor* ItemActor = World->SpawnActorDeferred<AGSWorldItemActor>(
														WorldItemClass,
														SpawnTransform,
														nullptr,
														nullptr,
														ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
	{
		ItemActor->GetItemComponent()->SetItemTag(ItemTag);
		ItemActor->GetItemComponent()->EnableRandomizer();
		ItemActor->bLoadDataManually = true;
		UGameplayStatics::FinishSpawningActor(ItemActor, SpawnTransform);			
	}
}
