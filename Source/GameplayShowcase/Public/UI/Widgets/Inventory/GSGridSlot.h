// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "GSGridSlot.generated.h"

class UBorder;

DECLARE_DELEGATE_TwoParams(FCheckAllGridItemPositionsSignature, const FGridPosition& Position, const FItemSize& inProxySize);
DECLARE_DELEGATE(FClearAllGridItemPositionsSignature);

/**
 * Represents a single grid cell within the inventory.
 * Responsible for tracking its occupancy, handling mouse events,
 * and updating visual feedback during GridItem drag.
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Sets the current occupancy state of this slot. */
	FORCEINLINE void SetOccupancyStatus(bool bIsOccupied);
	
	/** Returns true if there is currently a GridItem on this slot. */
	FORCEINLINE bool IsOccupied() const { return bOccupied; };

	/** Returns this slot's grid position coordinates. */
	FORCEINLINE FGridPosition GetPosition() const { return Position; };

	/** Assigns logical grid coordinates for this slot. */
	FORCEINLINE void SetPosition(int32 RowIndex, int32 ColumnIndex)
	{
		Position = FGridPosition(RowIndex, ColumnIndex);
	};

	/** Returns the Inventory grid index this item belongs to. */
	FORCEINLINE int32 GetInventoryGridIndex() const { return InventoryGridIndex; };
	FORCEINLINE void SetInventoryGridIndex(int32 Index) { InventoryGridIndex = Index; };
	
	/**
	 * Updates internal proxy tracking when a GridItem starts/stops being dragged.
	 *
	 * @param bProxyExists	true if a GridItem proxy is currently active.
	 * @param inProxySize	size of the dragged proxy in grid units (rows x columns).
	 */
	void SetGridItemProxyStatus(bool bProxyExists, const FItemSize& inProxySize);

	/**
	 * Visually indicates whether relocation is allowed or forbidden
	 * when the GridItem proxy hovers over this slot.
	 *
	 * @param bCanRelocateItem	true if GridItem can be placed here.
	 */
	void SetRelocationStatus(bool bCanRelocateItem);

	/** Resets this slot’s visual color to default state. */
	void ClearSlot();

	/** Delegate fired to check if GridItem can occupy all target positions (neighbors of this slot). */
	FCheckAllGridItemPositionsSignature CheckAllGridItemPositionsDelegate;

	/** Delegate fired to clear all grid slots that was currently hovered by a proxy. */
	FClearAllGridItemPositionsSignature ClearAllGridItemsPositionsDelegate;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(EditAnywhere)
	FLinearColor DefaultColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere)
	FLinearColor RelocationForbiddenColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere)
	FLinearColor RelocationAllowedColor = FLinearColor::White;

	bool bItemProxyExists = false;
	
private:	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> GridSlot;
	
	FGridPosition Position;
	FItemSize ProxySize;
	int32 InventoryGridIndex = 0;
	
	bool bOccupied = false;
	bool bRelocationAllowed = false;
};
