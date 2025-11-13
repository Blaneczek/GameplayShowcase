// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanelSlot.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSGridItem.generated.h"

class UGSInventoryMenuWidgetController;
class UCanvasPanelSlot;
class UGSGridItemProxy;
class UGSDragWidget;
class UImage;
struct FItemDefinition;


/**
 * Represents an inventory item displayed in a grid within the inventory UI.
 * Handles visual construction of the item (icon, size, and position),
 * spawning and interaction with the draggable GridItem proxy.
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridItem : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	/**
	 * Initializes this grid item’s visual representation and sets up its controller bindings.
	 * @param InInventoryGridIndex	index of the Inventory grid to which this GridItem belongs.
	 * @param Item					item instance containing item data.
	 * @param inSlotSize				size of a single grid slot (UI units).
	 * @param Positions				array of grid positions occupied by this GridItem.
	 */
	void ConstructGridItem(int32 InInventoryGridIndex, const FItemInstance& Item, float inSlotSize, const TArray<FGridPosition>& Positions);

	/** Returns the GridItem’s size in grid units (rows x columns). */
	FORCEINLINE FItemSize GetGridItemSize() const { return GridItemSize; }
	
	/** Returns the widget’s canvas slot size, or zero if none. */
	FORCEINLINE FVector2D GetCanvasSlotSize() const { return CanvasSlotRef ? CanvasSlotRef->GetSize() : FVector2D::ZeroVector; }

	/** Returns the GridItem’s current drag proxy widget, if any. */
	FORCEINLINE UGSGridItemProxy* GetItemProxy() const { return ItemProxy; }

	/** Returns the array of grid positions currently occupied by this item. */
	FORCEINLINE const TArray<FGridPosition>& GetGridPositions() const { return GridPositions; }

	/** Sets new grid positions using move semantics. */
	FORCEINLINE void SetGridPositions(TArray<FGridPosition>&& Positions)  { GridPositions = MoveTemp(Positions); }

	/** Returns the canvas slot of this widget. */
	FORCEINLINE UCanvasPanelSlot* GetCanvasSlot() const { return CanvasSlotRef; }
	
	/** Sets the canvas slot used by this widget. */
	FORCEINLINE void SetCanvasSlot(UCanvasPanelSlot* CanvasSlot) { CanvasSlotRef = CanvasSlot; }

	/** Returns the item’s unique identifier. */
	FORCEINLINE FGuid GetItemID() const { return ItemID; }

	/** Returns the item’s gameplay tag type. */
	FORCEINLINE FGameplayTag GetItemType() const { return ItemType; }

	/** Returns the Inventory grid index this item belongs to. */
	FORCEINLINE int32 GetInventoryGridIndex() const { return InventoryGridIndex; }

	/** Removes and cleans up the drag proxy widget (if active). */
	void RemoveProxy();

	/** Clears all grid slot positions occupied by this GridItem. */
	void ResetGridPositions();
	
	bool bIsEquipped = false;
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
private:
	void BindToInventoryController();
	
	/** Sets GridItem size in grid units (rows x columns). */
	void SetGridItemSize(const FGridFragment* GridFragment);
	
	void SetGridItemIcon(const FImageFragment* ImageFragment);
	
	/** Spawns a UGSGridItemProxy widget and attaches it to the overlay canvas panel. */
	void CreateItemProxy();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	/** Alternative icon texture used when the primary item icon is missing or invalid. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> AltIcon;

	/** Currently active instance of the draggable item proxy. */
	UPROPERTY()
    TObjectPtr<UGSGridItemProxy> ItemProxy;
	
	UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGSGridItemProxy> ItemProxyClass;	
	
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasSlotRef;

	/** Array of grid coordinates occupied by this GridItem. */
	TArray<FGridPosition> GridPositions;

	TWeakObjectPtr<UGSInventoryMenuWidgetController> CachedInventoryController;

	/** Unique ID of the item instance represented by this GridItem. */
	FGuid ItemID;
	
	/** Type of the item instance represented by this GridItem. */
	FGameplayTag ItemType;
	
	/** Stack count of the item instance represented by this GridItem. */
	int32 StackCount;
	
	float SlotSize = 0.f;
	int32 InventoryGridIndex;
	FItemSize GridItemSize;	
};

