// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "GSInventoryMenuWidgetController.generated.h"

class UGSGridItemProxy;
class UGSGridSlot;
class UGSGridItem;
struct FItemSize;

/**
 * Stores inventory grid index and occupied positions for a GridItem within Inventory Grid.
 */
USTRUCT(BlueprintType)
struct FGridInfo
{
	GENERATED_BODY()
	
	FGridInfo() = default;
	FGridInfo(int32 inGridIndex, const TArray<FGridPosition>& inPositions)
		: InventoryGridIndex(inGridIndex), Positions(inPositions)
	{}

	/** Which inventory grid this item is in */
	UPROPERTY(BlueprintReadOnly)
	int32 InventoryGridIndex = 0;

	/** Grid positions occupied by this item */
	UPROPERTY(BlueprintReadOnly)
	TArray<FGridPosition> Positions;
};

// GridItem delegates
DECLARE_DELEGATE_TwoParams(FCreateGridItemSignature, const FItemInstance* Item, const FGridInfo& GridInfo);
DECLARE_DELEGATE_TwoParams(FRelocateGridItemSignature, UGSGridItem* GridItem, int32 InventoryGridIndex);

// Equipping delegates
DECLARE_DELEGATE_TwoParams(FEquipGridItemSignature, UGSGridItem* GridItem,  UGSGridSlot* EquipSlot);
DECLARE_DELEGATE_RetVal_OneParam(UGSGridSlot*, FFindEquipSlotSignature, const FGameplayTag& EquipType);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FCheckEquipGridItemSignature, const FGameplayTag& EquipType, UGSGridSlot* ClickedSlot, UGSGridSlot*& OutEquipSlot);
DECLARE_DELEGATE_RetVal_OneParam(bool, FTryUnequipGridItemSignature, UGSGridItem* GridItem);

// Proxy delegates
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemProxyStatusChangedSignature, bool bProxyExists, const FItemSize& ProxySize);

// Other
DECLARE_DELEGATE_RetVal_TwoParams(bool, FFindNewSpaceSignature, const FItemSize& ItemSize, FGridInfo& OutGridInfo);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStackCountChangedSignature, const FGuid& InstanceID, int32 NewStackCount);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedSignature, const FGuid& InstanceID);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerLevelChangedSignatureInv, int32 PlayerLevel);

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

	/** 
	 * Finds free space in inventory grid that can fit the given item size.
	 * @param ItemSize		size of item in grid slots
	 * @param OutGridInfo	filled with grid location if found
	 * @return				true if space was found
	 */
	bool FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo);

	/** 
	 * Tries activate item's default action (equip, consume, etc.)
	 * @param ItemID	Unique ID of item instance
	 */
	void TryActivateItemAction(const FGuid& ItemID);

	/** Discards currently selected item from inventory. */
	void DiscardItem();

	/**
	 * Relocates currently selected GridItem when moving manually (by proxy).
	 * @param InventoryGridIndex	target grid index
	 */
    void RelocateGridItem(int32 InventoryGridIndex);

	/** Called when UI slot is clicked while a proxy is active - attempts to equip the currently selected GridItem. */
    void TryEquipGridItem(UGSGridSlot* ClickedSlot);
	
	/** Equips current GridItem into a slot found by EquipType. */
    void EquipGridItem(const FGameplayTag& EquipType);
	
	/** Equips current GridItem into a specific EquipSlot. */
    void EquipGridItem(UGSGridItem* GridItem, UGSGridSlot* EquipSlot);
	
	/** Tries to unequip current GridItem. If success, call Inventory component. */
    void TryUnequipGridItem();
	
	/**
	 * Tries to add currently selected item to target stack.
	 * @param ItemIDToAdd	ID of stack to add to
	 */
	void TryAddToStack(const FGuid& ItemIDToAdd);

	/**
	 * Checks if currently selected item can be added to target stack.
	 * @param ItemIDToAdd	ID of stack to check
	 * @return				true if items can be stacked together
	 */
    bool CheckIfCanAddToStack(const FGuid& ItemIDToAdd);
	
	/** Broadcast proxy status change to all UI elements (OnItemProxyStatusChanged). */
	void BroadcastProxyStatusChanged(bool bProxyExists, const FItemSize& ProxySize);

	void RemoveActiveGridItemProxy();
    
    FORCEINLINE bool IsProxyActive() const { return bProxyActive; };
	
	/** Set weak reference to GridItem being currently selected. */
	void SetGridItemRef(UGSGridItem* GridItem);
	
	FCreateGridItemSignature CreateNewItemDelegate;
	FRelocateGridItemSignature RelocateGridItemDelegate;
	FEquipGridItemSignature EquipGridItemDelegate;
	FFindEquipSlotSignature FindEquipGridSlotDelegate;
	FCheckEquipGridItemSignature CanEquipGridItemDelegate;
	FTryUnequipGridItemSignature TryUnequipGridItemDelegate;	
	FFindNewSpaceSignature FindNewSpaceDelegate;
	FItemProxyStatusChangedSignature OnItemProxyStatusChanged;
	FOnStackCountChangedSignature OnStackCountChanged;
	FOnItemRemovedSignature OnItemRemoved;
	FOnPlayerLevelChangedSignatureInv OnPlayerLevelChanged;
	
private:
	TWeakObjectPtr<UGSInventoryComponent> InventoryComponent;
	TWeakObjectPtr<UGSGridItem> SelectedGridItem;

	bool bProxyActive = false;
};
