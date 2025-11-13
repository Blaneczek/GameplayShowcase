// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Fragments/GSFragmentTags.h"
#include "UI/Widgets/Inventory/GSGridItem.h"
#include "UI/Widgets/Inventory/GSGridSlot.h"

void UGSInventoryMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
}

void UGSInventoryMenuWidgetController::BindCallbacksToDependencies()
{
	InventoryComponent = UGSInventoryComponent::FindInventoryComponent(Character);
	if (UGSInventoryComponent* InvComponent = InventoryComponent.Get())
	{
		InvComponent->OnItemInstanceAddedDelegate.BindLambda([this](const FItemInstance& Item, const FGridInfo& GridInfo)
		{
			CreateNewItemDelegate.ExecuteIfBound(Item, GridInfo);
		});
		InvComponent->OnItemEquippedDelegate.BindUObject(this, &UGSInventoryMenuWidgetController::EquipGridItem);
	}
}

bool UGSInventoryMenuWidgetController::TryFindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	if (FindNewSpaceDelegate.IsBound())
	{
		return FindNewSpaceDelegate.Execute(ItemSize, OutGridInfo);
	}
	return false;
}

void UGSInventoryMenuWidgetController::CallOnGridItemProxyStatusChanged(bool bProxyExists, const FItemSize& ProxySize)
{
	OnItemProxyStatusChanged.Broadcast(bProxyExists, ProxySize);
}

void UGSInventoryMenuWidgetController::RelocateGridItem(int32 InventoryGridIndex)
{
	UGSGridItem* GridItem = GridItemRef.Get();	
	UGSInventoryComponent* InvComponent = InventoryComponent.Get();
	if (!GridItem || !InvComponent)
	{
		return;
	}

	if (GridItem->bIsEquipped)
	{
		InvComponent->UnequipItem(GridItem->GetItemID());		
	}
	RelocateGridItemDelegate.ExecuteIfBound(GridItem, InventoryGridIndex);
    GridItem->RemoveProxy();	
}

void UGSInventoryMenuWidgetController::SetGridItemRef(UGSGridItem* GridItem)
{
	GridItemRef = GridItem;
}

void UGSInventoryMenuWidgetController::TryActivateItemAction(const FGuid& ItemID)
{
	if (UGSInventoryComponent* InvComponent = InventoryComponent.Get())
	{
		InvComponent->TryActivateItemAction(ItemID);
	}
}

void UGSInventoryMenuWidgetController::TryEquipGridItem(UGSGridSlot* ClickedSlot)
{
	UGSGridItem* GridItem = GridItemRef.Get();	
	UGSInventoryComponent* InvComponent = InventoryComponent.Get();
	if (!GridItem || !InvComponent || !CanEquipGridItemDelegate.IsBound())
	{
		return;
	}

	UGSGridSlot* EquipSlot = nullptr;
	if (CanEquipGridItemDelegate.Execute(GridItem->GetItemType(), ClickedSlot, EquipSlot))
	{
		if (InvComponent->TryEquipItem(GridItem->GetItemID()))
		{
			EquipGridItem(EquipSlot);
		}
	}
}

void UGSInventoryMenuWidgetController::TryUnequipGridItem()
{
	UGSGridItem* GridItem = GridItemRef.Get();	
	UGSInventoryComponent* InvComponent = InventoryComponent.Get();
	if (!GridItem || !InvComponent || !TryUnequipGridItemDelegate.IsBound())
	{
		return;
	}
	
	if (TryUnequipGridItemDelegate.Execute(GridItem))
	{
		InvComponent->UnequipItem(GridItem->GetItemID());
	}
}

void UGSInventoryMenuWidgetController::EquipGridItem(const FGameplayTag& EquipType)
{
	if (FindNewSpaceDelegate.IsBound())
	{
		UGSGridSlot* EquipSlot = FindEquipGridSlotDelegate.Execute(EquipType);
		EquipGridItemDelegate.ExecuteIfBound(GridItemRef.Get(), EquipSlot);
	}	
}

void UGSInventoryMenuWidgetController::EquipGridItem(UGSGridSlot* EquipSlot)
{
	EquipGridItemDelegate.ExecuteIfBound(GridItemRef.Get(), EquipSlot);
	GridItemRef.Get()->RemoveProxy();
}
