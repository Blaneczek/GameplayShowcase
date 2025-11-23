// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "GSItemInstance.generated.h"

/**
 * Runtime instance of an item with unique ID.
 * Wraps an FItemDefinition with instance-specific data like stack count and GUID.
 * Instances represent actual items in Inventory.
 */
USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

	FItemInstance() = default;

	FORCEINLINE const FItemDefinition& GetItemDefinition() const {return ItemDefinition; }
	FORCEINLINE FItemDefinition& GetItemDefinitionMutable() { return ItemDefinition; }
	
	/** Checks if this instance has a valid item definition. */
	FORCEINLINE bool IsValidDefinition() const { return ItemDefinition.Name.IsValid() && InstanceID.IsValid(); }

	/** Gets the current stack count. */
	FORCEINLINE int32 GetStackCount() const { return StackCount; };

	/** Sets the stack count directly. Clamps to minimum of 0. */
	FORCEINLINE void SetStackCount(int32 InStackCount) { StackCount = FMath::Max(0, InStackCount); }

	/** Adds to the current stack. Can be negative to remove. Clamps result to minimum of 0. */
	FORCEINLINE void AddToStack(int32 NumToAdd) { StackCount = FMath::Max(0, StackCount + NumToAdd); }

	/** Gets the unique instance identifier. */
	FORCEINLINE FGuid GetInstanceID() const { return InstanceID; };

	/** Checks if this instance has a valid ID. */
	FORCEINLINE bool HasValidID() const { return InstanceID.IsValid(); }

	/** Checks if two instances refer to the same unique item (by GUID). */
	FORCEINLINE bool operator==(const FItemInstance& Other) const
	{
		return (*this == Other);
	}
	
	/**
	 * Initialize this instance with a definition and initial stack count. Move semantics version.
	 * @param NewItemDefinition		The item definition to use
	 * @param InitialStackCount		Starting stack count
	 */
	void Initialize(FItemDefinition&& NewItemDefinition, int32 InitialStackCount = 1);
	
	/**
	 * Initialize this instance with a definition and initial stack count.
	 * @param NewItemDefinition		The item definition to use
	 * @param InitialStackCount		Starting stack count
	 */
	void Initialize(const FItemDefinition& NewItemDefinition, int32 InitialStackCount);
	
private:
	/** Load data for all fragments in the item definition. */
	void LoadItemData();

	UPROPERTY(EditAnywhere)
	FItemDefinition ItemDefinition;

	/** Current stack count for this instance. */
	UPROPERTY(VisibleAnywhere)
	int32 StackCount = 0;

	/** Unique identifier for this specific item instance. */
	UPROPERTY()
	FGuid InstanceID; 
};

