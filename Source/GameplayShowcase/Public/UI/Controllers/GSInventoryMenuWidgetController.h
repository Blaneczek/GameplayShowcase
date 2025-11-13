// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "GSInventoryMenuWidgetController.generated.h"

class UGSGridSlot;
class UGSGridItem;
struct FItemSize;

/**
 * Stores inventory grid index and occupied positions for a GridItem within Inventory Grid.
 */
struct FGridInfo
{
	FGridInfo() = default;
	FGridInfo(int32 inGridIndex, const TArray<FGridPosition>& inPositions)
		: InventoryGridIndex(inGridIndex), Positions(inPositions)
	{}
	
	int32 InventoryGridIndex = 0;
	TArray<FGridPosition> Positions;
};

// GridItem delegates
DECLARE_DELEGATE_TwoParams(FCreateGridItemSignature, const FItemInstance& Item, const FGridInfo& GridInfo);
DECLARE_DELEGATE_TwoParams(FRelocateGridItemSignature, UGSGridItem* GridItem, int32 InventoryGridIndex);

// Equipping delegates
DECLARE_DELEGATE_TwoParams(FEquipGridItemSignature, UGSGridItem* GridItem,  UGSGridSlot* EquipSlot);
DECLARE_DELEGATE_RetVal_OneParam(UGSGridSlot*, FFindEquipGridSlotSignature, const FGameplayTag& EquipType);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FCheckEquipGridItemSignature, const FGameplayTag& EquipType, UGSGridSlot* ClickedSlot, UGSGridSlot*& OutEquipSlot);
DECLARE_DELEGATE_RetVal_OneParam(bool, FTryUnequipGridItemSignature, UGSGridItem* GridItem);

// Proxy delegates
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemProxyStatusChangedSignature, bool bProxyExists, const FItemSize& ProxySize);

DECLARE_DELEGATE_RetVal_TwoParams(bool, FFindNewSpaceDelegate, const FItemSize& ItemSize, FGridInfo& OutGridInfo);


/**
 * Controller for the Inventory Menu Widget.
 * Handles communication between UI and InventoryComponent 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYSHOWCASE_API UGSInventoryMenuWidgetController : public UGSWidgetControllerBase
{
	GENERATED_BODY()

public:
	//~ Begin UGSWidgetControllerBase
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	//~ End UGSWidgetControllerBase

	/** Attempts to find a free grid space that can fit the given item. 
	* 
	* @param ItemSize		size of the item (in grid slots).
	* @param OutGridInfo	grid info filled in if a space was found.				
	* @return				true if space was found, false if wasn't.
	*/
	bool TryFindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo);

	/** Ask InventoryComponent to activate default action of item (equip, consume, etc.).
	* 
	* @param ItemID			unique ID of item instance represented in the UI			
	*/
	void TryActivateItemAction(const FGuid& ItemID);

	/** Called when UI slot is clicked while a proxy is active - attempt to equip the currently selected GridItem. */
    void TryEquipGridItem(UGSGridSlot* ClickedSlot);

	/** Broadcast proxy existence to entire inventory UI (OnItemProxyStatusChanged). */
	void CallOnGridItemProxyStatusChanged(bool bProxyExists, const FItemSize& ProxySize);

	/** Relocate currently selected GridItem when moving manually (by proxy). */
	void RelocateGridItem(int32 InventoryGridIndex);
	
	/** Set weak reference to GridItem being currently selected. */
	void SetGridItemRef(UGSGridItem* GridItem);

	/** Try to unequip current GridItem. If success, call Inventory component. */
	void TryUnequipGridItem();

	/** Equip current GridItem into a slot found by EquipType. */
	void EquipGridItem(const FGameplayTag& EquipType);

	/** Equip current GridItem into a specific EquipSlot. */
	void EquipGridItem(UGSGridSlot* EquipSlot);

	FCreateGridItemSignature CreateNewItemDelegate;
	FRelocateGridItemSignature RelocateGridItemDelegate;

	FEquipGridItemSignature EquipGridItemDelegate;
	FFindEquipGridSlotSignature FindEquipGridSlotDelegate;
	FCheckEquipGridItemSignature CanEquipGridItemDelegate;
	FTryUnequipGridItemSignature TryUnequipGridItemDelegate;
	
	FFindNewSpaceDelegate FindNewSpaceDelegate;
	FItemProxyStatusChangedSignature OnItemProxyStatusChanged;
		
private:
	TWeakObjectPtr<UGSInventoryComponent> InventoryComponent;
	TWeakObjectPtr<UGSGridItem> GridItemRef;
};
