// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSInventoryHelper.h"
#include "Components/ActorComponent.h"
#include "Items/GSItemInstance.h"
#include "GSInventoryComponent.generated.h"

struct FGridInfo;
class AGSPlayerCharacterBase;
class UGSWidgetBase;

/**
 * Position in the Inventory grid.
 */
USTRUCT(BlueprintType)
struct FGridPosition
{
	GENERATED_BODY()

	FGridPosition() = default;
	FGridPosition(int32 Rows, int32 Columns) : RowsIndex(Rows), ColumnsIndex(Columns) {}

	/** Spaceship operator for automatic comparison operators. */
	auto operator<=>(const FGridPosition& Other) const = default;
	
	UPROPERTY(EditAnywhere)
	int32 RowsIndex = 0;
	UPROPERTY(EditAnywhere)
	int32 ColumnsIndex = 0;
};

DECLARE_DELEGATE_RetVal_OneParam(bool, FTryChangeEquipItemStatusSignature, FItemInstance* /*Instance*/);
DECLARE_DELEGATE_TwoParams(FOnNewItemAddedSignature, const FItemInstance* /*Instance*/, const FGridInfo& /*GridInfo*/);
DECLARE_DELEGATE_OneParam(FOnItemStatusChangedByIDSignature, const FGuid& /*ItemID*/);
DECLARE_DELEGATE_OneParam(FOnItemStatusChangedByInstanceSignature, FItemInstance* /*Instance*/);
DECLARE_DELEGATE_OneParam(FOnItemEquippedSignature, const FGameplayTag& /*EquipType*/);


/**
 * Main component managing player inventory.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSInventoryComponent : public UActorComponent, public IGSInventoryHelper
{
	GENERATED_BODY()

public:
	UGSInventoryComponent();

	/** Finds inventory component on an actor. Returns nullptr if not found. */
	static UGSInventoryComponent* FindInventoryComponent(AActor* Actor);

	/* IGSInventoryHelper interface */
	virtual void AddItemOnFloor(UGSItemComponent* ItemComponent) override;
	virtual void RemoveItemOnFloor(UGSItemComponent* ItemComponent) override;
	/***/

	/** Finds item instance by unique ID. Returns nullptr if not found. */
	FItemInstance* FindItemInstanceByID(const FGuid& ItemID);
	
	/** Finds first item instance by gameplay tag name. Returns nullptr if not found. */
	FItemInstance* FindItemInstanceByNameTag(const FGameplayTag& ItemName);

	/** Finds item instance using custom predicate. Returns nullptr if not found. */
	FItemInstance* FindItemInstanceByPredicate(TFunctionRef<bool(const FItemInstance&)> Predicate);

	/** Gets all item instances in inventory. */
	FORCEINLINE const TArray<FItemInstance>& GetAllItems() const { return ItemInstances; }

	/** Attempts to pick up item on the floor. */
	void TryAddItem();

	/** Adds items from one stack to another. */
	void TryAddToItemStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom);
	
	/** Checks if items can be stacked together. */
	bool CheckIfCanAddToStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom);
	
	/** Attempts to activate item action (equip, consume, etc.) based on item type. */
    bool TryActivateItemAction(const FGuid& ItemID);

	/** Attempts to equip item. Returns true if successful. */
	bool TryEquipItem(FItemInstance* Item); 
	bool TryEquipItem(const FGuid& ItemID);

	/** Unequips item. */
	void UnequipItem(const FGuid& ItemID);

	/** Discards item from inventory, spawning it in the world. */
	void DiscardItemInstance(const FGuid& ItemID);
	
	FTryChangeEquipItemStatusSignature TryEquipItemDelegate;
	FOnItemEquippedSignature OnItemEquippedDelegate;
	FOnItemStatusChangedByInstanceSignature UnequipItemDelegate;
	
	FOnNewItemAddedSignature OnItemInstanceAddedDelegate;	
	FOnItemStatusChangedByIDSignature OnItemInstanceRemovedDelegate;
	FOnItemStatusChangedByInstanceSignature OnItemInstanceChangedDelegate;		
		
protected:
	virtual void BeginPlay() override;

	/** Maximum number of items that can be in a single stack. */
	UPROPERTY(EditAnywhere, meta=(ClampMin="1"))
	int32 MaxStackSize = 100;
	
private:
	/** Tries to add item to existing stack. Returns true if added. */
	bool TryAddNewItemToStack(FItemDefinition& Def) ;

	/** Tries to add item as new inventory instance.
	 * @returns true if added, false if free space was not found
	 */
	bool TryAddNewItem(FItemDefinition& Def);
	
	/** Remove item instance from inventory. */
	void RemoveItemInstance(FItemInstance* Item);
	void RemoveItemInstance(const FGuid& ItemID);

	/** Spawn item actor in the world. */
	void SpawnWorldItemActor(FItemDefinition&& MovedItemDef, const UObject* WorldContextObject, const FVector& SpawnLocation);

	/** 
	 * Changes stack count, respecting max stack size.
	 * @return Remaining amount that couldn't be added
	 */
	int32 HandleStackChange(FItemInstance* Instance, int32 ChangeNum);

	/** Create a new item instance from definition. */
	FItemInstance CreateItemInstance(FItemDefinition& Def);

	/** All items currently in inventory. */
	UPROPERTY()
	TArray<FItemInstance> ItemInstances;

	/** Cached reference to owning character. */
	TWeakObjectPtr<AGSPlayerCharacterBase> OwningCharacter;

	/** Items on floor near player that can be picked up. */
	UPROPERTY()
	TSet<TWeakObjectPtr<UGSItemComponent>> ItemsOnFloor;
};
