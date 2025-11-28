// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Fragments/GSFragmentTags.h"
#include "Systems/Leveling/GSLevelingComponent.h"
#include "UI/Widgets/Inventory/GSGridItem.h"
#include "UI/Widgets/Inventory/GSGridSlot.h"

void UGSInventoryMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	
	if (const UGSLevelingComponent* LevelComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		OnPlayerLevelChanged.Broadcast(LevelComponent->GetLevel());
	}
} 

void UGSInventoryMenuWidgetController::BindCallbacksToDependencies()
{
	InventoryComponent = UGSInventoryComponent::FindInventoryComponent(Character);
	if (UGSInventoryComponent* InvComponent = InventoryComponent.Get())
	{
		InvComponent->OnItemInstanceAddedDelegate.BindLambda([this](const FItemInstance* Item, const FGridInfo& GridInfo)
		{
			CreateNewItemDelegate.ExecuteIfBound(Item, GridInfo);
		});
		InvComponent->OnItemInstanceChangedDelegate.BindLambda([this](FItemInstance* Item)
		{
			if (Item)
			{
				OnStackCountChanged.Broadcast(Item->GetInstanceID(), Item->GetStackCount());
			}		
		});
		InvComponent->OnItemInstanceRemovedDelegate.BindLambda([this](const FGuid& ItemID)
		{
			OnItemRemoved.Broadcast(ItemID);
		});
		InvComponent->OnItemEquippedDelegate.BindUObject(this, &UGSInventoryMenuWidgetController::EquipGridItem);
	}

	if (UGSLevelingComponent* LevelComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		LevelComponent->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
		{
			OnPlayerLevelChanged.Broadcast(NewLevel);
		});
	}
}

bool UGSInventoryMenuWidgetController::FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	if (FindNewSpaceDelegate.IsBound())
	{
		return FindNewSpaceDelegate.Execute(ItemSize, OutGridInfo);
	}
	return false;
}

void UGSInventoryMenuWidgetController::TryActivateItemAction(const FGuid& ItemID)
{
	if (UGSInventoryComponent* InvComponent = InventoryComponent.Get())
	{
		InvComponent->TryActivateItemAction(ItemID);
	}
}

void UGSInventoryMenuWidgetController::DiscardItem()
{
	const UGSGridItem* GridItem = SelectedGridItem.Get();	
	UGSInventoryComponent* InvComponent = InventoryComponent.Get();
	if (!GridItem || !InvComponent)
	{
		return;
	}

	InvComponent->DiscardItemInstance(GridItem->GetItemID(), GridItem->bIsEquipped);
}

void UGSInventoryMenuWidgetController::RelocateGridItem(int32 InventoryGridIndex)
{
	UGSGridItem* GridItem = SelectedGridItem.Get();	
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

void UGSInventoryMenuWidgetController::TryEquipGridItem(UGSGridSlot* ClickedSlot)
{
	UGSGridItem* GridItem = SelectedGridItem.Get();	
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
			EquipGridItem(GridItem, EquipSlot);
		}
	}
}

void UGSInventoryMenuWidgetController::EquipGridItem(const FGameplayTag& EquipType)
{
	if (FindEquipGridSlotDelegate.IsBound())
	{
		UGSGridSlot* EquipSlot = FindEquipGridSlotDelegate.Execute(EquipType);
		EquipGridItemDelegate.ExecuteIfBound(SelectedGridItem.Get(), EquipSlot);
	}	
}

void UGSInventoryMenuWidgetController::EquipGridItem(UGSGridItem* GridItem, UGSGridSlot* EquipSlot)
{
	EquipGridItemDelegate.ExecuteIfBound(GridItem, EquipSlot);
	GridItem->RemoveProxy();
}

void UGSInventoryMenuWidgetController::TryUnequipGridItem()
{
	UGSGridItem* GridItem = SelectedGridItem.Get();	
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

void UGSInventoryMenuWidgetController::TryAddToStack(const FGuid& ItemIDToAdd)
{
	UGSGridItem* GridItem = SelectedGridItem.Get();
	UGSInventoryComponent* InvComponent = InventoryComponent.Get();
	if (!GridItem || !InvComponent)
	{
		return;
	}
	GridItem->RemoveProxy();
	InvComponent->TryAddToItemStack(ItemIDToAdd, GridItem->GetItemID());
}

bool UGSInventoryMenuWidgetController::CheckIfCanAddToStack(const FGuid& ItemIDToAdd)
{
	const UGSGridItem* GridItem = SelectedGridItem.Get();
	UGSInventoryComponent* InvComponent = InventoryComponent.Get();
	if (!GridItem || !InvComponent)
	{
		return false;
	}

	return InvComponent->CheckIfCanAddToStack(ItemIDToAdd, GridItem->GetItemID());
}

void UGSInventoryMenuWidgetController::BroadcastProxyStatusChanged(bool InProxyExists, const FItemSize& ProxySize)
{
	bProxyActive = InProxyExists;
	OnItemProxyStatusChanged.Broadcast(bProxyActive, ProxySize);
}

void UGSInventoryMenuWidgetController::RemoveActiveGridItemProxy()
{
	if (UGSGridItem* GridItem = SelectedGridItem.Get())
	{
		GridItem->RemoveProxy();
	}
}

void UGSInventoryMenuWidgetController::SetGridItemRef(UGSGridItem* GridItem)
{
	SelectedGridItem = GridItem;
}