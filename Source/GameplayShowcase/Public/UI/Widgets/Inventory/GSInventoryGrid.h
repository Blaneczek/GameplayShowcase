// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSInventoryGrid.generated.h"

class UCanvasPanel;
struct FItemDefinition;
class UGSGridItem;
struct FGridPosition;
class UGSGridSlot;


/**
 * Represents a single visual inventory grid - a matrix of slots that can hold GridItems.
 * Handles construction of slot widgets, placement and relocation of GridItems, and finding free space in grid.
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSInventoryGrid : public UGSWidgetBase
{
	GENERATED_BODY()
	
public:
	/** Returns a const reference to the currently highlighted slot positions hovered by proxy. */
	FORCEINLINE const TArray<FGridPosition>& GetProxyPositions() const { return ProxyPositions; }

	/** Moves the proxy positions array, clearing ownership. */
	FORCEINLINE TArray<FGridPosition>&& MoveProxyPositions() { return MoveTemp(ProxyPositions); }

	/** Returns the internal canvas panel. */
    FORCEINLINE UCanvasPanel* GetGridPanel() const { return GridPanel; }

	/** Returns the grid's index. */
	FORCEINLINE int32 GetInventoryGridIndex () const { return InventoryGridIndex; }
	
	/** Adds a new GridItem to the grid and marks its occupied positions. */
	void AddNewGridItem(const FItemInstance* Item, const TArray<FGridPosition>& Positions);

	/** Moves a GridItem to new grid positions, updating occupancy and canvas position. */
	void RelocateGridItem(UGSGridItem* GridItem, TArray<FGridPosition>&& Positions);

	/** Removes a GridItem form Inventory grid and frees its occupied slots without deleting it. */
	void RemoveGridItemTemporary(UGSGridItem* GridItem);

	/** Removes a GridItem form Inventory grid, frees its occupied slots and deletes it. */
	void RemoveGridItemPermanently(UGSGridItem* GridItem);

	/** Finds a free space large enough to fit an item of given size.
	 *
	 * @param ItemSize		size of the item in grid units (rows x columns)
	 * @param OutGridInfo	information about the found grid and positions (if successful).
	 * @returns				true if free space was found.	
	 */
	bool FindFreeSpaceForItem(const FItemSize& ItemSize, FGridInfo& OutGridInfo);

	/** Sets occupancy flag for multiple grid positions. */
	void SetSlotsOccupancy(const TArray<FGridPosition>& Positions, bool bOccupied, UGSGridItem* OccupyingGridItem = nullptr);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> GridPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 InventoryGridIndex = 0;
	
private:
	UFUNCTION(BlueprintCallable)
	void ConstructGrid();

	/** Checks if all positions for a GridItem of given size are valid and unoccupied. */
	bool CheckAllPositions(int32 RowIndex, int32 ColumnIndex, const FItemSize& ItemSize, TArray<FGridPosition>& Positions);

	/** Returns the grid slot at specified coordinates (nullptr if invalid). */
	UGSGridSlot* GetGridSlotAtPosition(int32 RowIndex, int32 ColumnIndex);

	/** Handles slots highlight status that are currently hovered by proxy. */
	void SetProxyPositionsStatus(const FGridPosition& Position, const FItemSize& ProxySize);

	/** Clears all highlighted slots previously hovered by proxy. */
	void ClearProxyPositions();
	
	/** Returns the top-left grid position from an array of positions. */
	FGridPosition GetFirstGridPosition(const TArray<FGridPosition>& Positions);

	bool CheckIfCanAddToStack(const FGridPosition& Position);
	
	/** All grid slot widgets belonging to this grid. */
	UPROPERTY()
	TArray<UGSGridSlot*> GridSlots;

	/** All GridItem widgets currently displayed in this grid. */
	UPROPERTY()
	TArray<UGSGridItem*> GridItems;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GridSlotClass;	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSGridItem> GridItemClass;

	/** Temporary positions highlighted when hovered by proxy. */
	TArray<FGridPosition> ProxyPositions;

	TWeakObjectPtr<UGSInventoryMenuWidgetController> CachedInventoryController;
	
	UPROPERTY(EditAnywhere)
	int32 RowsNum = 0;
	UPROPERTY(EditAnywhere)
	int32 ColumnsNum = 0;
	UPROPERTY(EditAnywhere)
	float SlotSize = 0.f;
};
