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
	const AActor* CachedOwner = GetOwner();
	if (!CachedOwner)
	{
		return;
	}

	const FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, HeightOffset);
	SpawnItemsAtLocation(SpawnLocation);
}

void UGSLootComponent::SpawnItemsAtLocation(const FVector& SpawnLocation)
{
	UWorld* CachedWorld = GetWorld();
	if (!CachedWorld || !WorldItemClass || !HasValidLootTable())
	{
		return;
	}

	for (const auto& LootEntry : LootTable)
	{
		if (!LootEntry.IsValid())
		{
			continue;
		}
		const float Roll = FMath::FRandRange(0.f, 100.f);
		if (Roll > LootEntry.DropChance)
		{
			continue;
		}

		const int32 Quantity = FMath::RandRange(LootEntry.QuantityRange.Min, LootEntry.QuantityRange.Max);
		if (Quantity <= 0)
		{
			continue;
		}
		for (int32 i = 0; i < Quantity; ++i)
		{
			const FVector RandomLocation = GetRandomSpawnLocation(SpawnLocation);
			SpawnWorldItem(CachedWorld, RandomLocation, LootEntry.ItemTag);
		}		
	}	
}

bool UGSLootComponent::HasValidLootTable() const
{
	if (LootTable.Num() == 0)
	{
		return false;
	}

	for (const FLootInfo& Entry : LootTable)
	{
		if (Entry.IsValid())
		{
			return true;
		}
	}
	return false;
}

void UGSLootComponent::SpawnWorldItem(UWorld* World, const FVector& SpawnLocation, const FGameplayTag& ItemTag)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	AGSWorldItemActor* ItemActor = World->SpawnActorDeferred<AGSWorldItemActor>(\
		WorldItemClass,
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	if (ItemActor)
	{
		UGSItemComponent* ItemComponent = ItemActor->GetItemComponent();
		if (!ItemComponent)
		{
			return;
		}
		ItemComponent->SetItemTag(ItemTag);
		ItemComponent->EnableRandomizer();
		ItemActor->bLoadDataAutomatically = true;
		UGameplayStatics::FinishSpawningActor(ItemActor, SpawnTransform);			
	}
}

FVector UGSLootComponent::GetRandomSpawnLocation(const FVector& BaseLocation) const
{
	const FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.0f, RadiusOffset);
	return BaseLocation + FVector(RandomOffset.X, RandomOffset.Y, 0.0f);
}
