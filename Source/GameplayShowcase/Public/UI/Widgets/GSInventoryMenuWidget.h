// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetBase.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "GSInventoryMenuWidget.generated.h"

class UCanvasPanel;
class UGSGridSlot;
struct FItemDefinition;
struct FGridPosition;
class UGSInventoryMenuWidgetController;
class UGSInventoryGrid;


/**
 * Main inventory menu widget responsible for managing and displaying
 * all player inventory grids and equipment slots in the UI.
 * This widget acts as the visual layer of the inventory system.
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSInventoryMenuWidget : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	/** 
	 * Finds free space in inventory grid that can fit the given item size.
	 * @param ItemSize		size of item in grid slots
	 * @param OutGridInfo	filled with grid location if found
	 * @return				true if space was found
	 */
	bool FindFreeSpaceForItem(const FItemSize& ItemSize, FGridInfo& OutGridInfo);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** Adds grids created in blueprint. */ 
	UFUNCTION(BlueprintCallable)
	void RegisterInventoryGrid(UGSInventoryGrid* NewInventoryGrid);
	/** Adds equip slots created in blueprint. */ 
	UFUNCTION(BlueprintCallable)
	void RegisterEquipGridSlots(TMap<FGameplayTag, UGSGridSlot*> InEquipSlots);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> EquipmentPanel;
	
private:
	/** Adds new GridItem into the appropriate grid. */
	void AddGridItem(const FItemInstance* Item, const FGridInfo& GridInfo);

	/**
	 * Relocates a GridItem to a new position.
	 * Used when moving GridItem manually (by proxy).
	 *
	 * @param GridItem				GridItem to move.
	 * @param InventoryGridIndex	target inventory grid index.
	 */
	void RelocateGridItem(UGSGridItem* GridItem, int32 InventoryGridIndex);

	/**
	 * Relocates a GridItem to a new position.
	 * Used when moving GridItem by item action (right mouse button).
	 *
	 * @param GridItem				GridItem to move.
	 * @param InventoryGridIndex	target inventory grid index.
	 * @param Positions				array of slot positions to occupy in the grid.
	 */
	void RelocateGridItem(UGSGridItem* GridItem, int32 InventoryGridIndex, TArray<FGridPosition>&& Positions);

	/**
	 * Checks whether the GridItem can be equipped into a specific slot.
	 *
	 * @param ItemType		gameplay tag representing the item’s type.
	 * @param ClickedSlot	slot the player interacted with.
	 * @param OutEquipSlot	output reference to the correct equip slot if valid.
	 * @return				true if the GridItem can be equipped into the slot.
	 */
	bool CanEquipItem(const FGameplayTag& ItemType, UGSGridSlot* ClickedSlot, UGSGridSlot*& OutEquipSlot);

	/**
	 * Attempts to unequip a GridItem and relocate it back into the inventory grid.
	 * Used when using item action (right mouse button).
	 *
	 * @param UnequippedGridItem	GridItem being unequipped.
	 * @return						true if the GridItem was successfully placed in inventory.
	 */
	bool TryUnequipGridItem(UGSGridItem* UnequippedGridItem);

	/** Visually equips a GridItem by moving it from the inventory grid to the equip slots. */
	void EquipGridItem(UGSGridItem* GridItem, UGSGridSlot* EquipSlot);

	/** Finds the equip slot widget that matches the given item type. */
	UGSGridSlot* FindEquipGridSlot(const FGameplayTag& EquipType);
	
	UPROPERTY()
	TMap<int32, UGSInventoryGrid*> InventoryGrids;	
	UPROPERTY()
	TMap<FGameplayTag, UGSGridSlot*> EquipSlots;

	TWeakObjectPtr<UGSInventoryMenuWidgetController> CachedInventoryController; 
};
