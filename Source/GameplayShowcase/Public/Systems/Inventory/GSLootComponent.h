// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GSLootComponent.generated.h"

class AGSWorldItemActor;

/**
 * Defines a single loot drop with drop chance and quantity range.
 */
USTRUCT(BlueprintType)
struct FLootInfo
{
	GENERATED_BODY()

	FLootInfo() = default;
	FLootInfo(const FGameplayTag& InItemTag, float InDropChance, const FInt32Interval& InQuantity)
		: ItemTag(InItemTag), DropChance(InDropChance), QuantityRange(InQuantity) {}

	/** Item name tag to drop. */
	UPROPERTY(EditAnywhere)
	FGameplayTag ItemTag = FGameplayTag::EmptyTag;

	/** Percentage chance this item drops (0-100). */
	UPROPERTY(EditAnywhere)
	float DropChance = 100.f;

	/** Number of items to spawn if drop succeeds. */
	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	FInt32Interval QuantityRange = FInt32Interval(1, 1);

	/** Check if this loot entry is valid */
	bool IsValid() const { return ItemTag.IsValid() && DropChance >= 0.0f && QuantityRange.Max >= QuantityRange.Min; }
};


/**
 * Component that manages loot drops for entities (enemies, chests, etc.).
 * Usage:
 * 1. Attach to actor that drops loot (enemy, chest, etc.)
 * 2. Configure LootTable with items and drop chances
 * 3. Set WorldItemClass to your world item actor
 * 4. Call SpawnItems() when entity dies/opens
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSLootComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSLootComponent();

	/** Finds loot component on an actor. Returns nullptr if not found. */
	UFUNCTION(BlueprintPure, Category="GS|LootComponent")
	static UGSLootComponent* FindLootComponent(const AActor* Actor);

	/**
	 * Rolls loot table and spawns all successful drops.
	 * Spawns items at owner's location with random offset.
	 */
	UFUNCTION(BlueprintCallable, Category="GS|LootComponent")
	void SpawnItems();

	/**
	 * Rolls loot table at a specific location.
	 * @param SpawnLocation	World location to spawn items
	 */
	UFUNCTION(BlueprintCallable, Category="GS|LootComponent")
	void SpawnItemsAtLocation(const FVector& SpawnLocation);

	/**
	 * Check if loot table is configured.
	 * @return	True if loot table has valid entries
	 */
	bool HasValidLootTable() const;
	
protected:
	UPROPERTY(EditAnywhere)
    TArray<FLootInfo> LootTable;
	
	UPROPERTY(EditAnywhere)
    TSubclassOf<AGSWorldItemActor> WorldItemClass;
	
	/** Height offset above owner position to spawn items. */
    UPROPERTY(EditAnywhere, Category = "Loot Configuration", meta=(ClampMin="0.0"))
    float HeightOffset = 50.f;

	/** Maximum horizontal offset distance for dropped items. */
	UPROPERTY(EditAnywhere, Category = "Loot Configuration", meta=(ClampMin="0.0"))
	float RadiusOffset = 100.0f;
	
private:
	void SpawnWorldItem(UWorld* World, const FVector& SpawnLocation, const FGameplayTag& ItemTag);

	FVector GetRandomSpawnLocation(const FVector& BaseLocation) const;
};
