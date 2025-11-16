// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "GSItemInstance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

	FItemInstance() = default;

	FORCEINLINE const FItemDefinition& GetItemDefinition() const {return ItemDefinition.Get(); }
	FORCEINLINE FItemDefinition& GetItemDefinitionMutable() { return ItemDefinition.GetMutable(); }
	
	FORCEINLINE int32 GetStackCount() const { return StackCount; };
	FORCEINLINE void SetStackCount(int32 InStackCount) { StackCount = InStackCount; };
	FORCEINLINE void AddToStack(int32 NumToAdd)
	{
		StackCount = FMath::Max(0, StackCount += NumToAdd);
	}
		
	FORCEINLINE FGuid GetInstanceID() const { return InstanceID; };
	FORCEINLINE void CreateID() { InstanceID = FGuid::NewGuid(); }
	
	void MoveItemDefinition(FItemDefinition&& NewItemDefinition);
	void CopyItemDefinition(const FItemDefinition& NewItemDefinition);

private:
	void LoadItemData();
	
	UPROPERTY(EditAnywhere)
	TInstancedStruct<FItemDefinition> ItemDefinition;
	
	UPROPERTY(VisibleAnywhere)
	int32 StackCount = 0;

	/** Used to differentiate between identical item types or stack entries. */
	UPROPERTY()
	FGuid InstanceID; 
};

