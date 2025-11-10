// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "UI/Widgets/Inventory/GSGridItem.h"
#include "UI/Widgets/Inventory/GSGridItemProxy.h"

void UGSInventoryMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
}

void UGSInventoryMenuWidgetController::BindCallbacksToDependencies()
{
	InventoryComponent = UGSInventoryComponent::FindInventoryComponent(Character);

	if ( UGSInventoryComponent* Comp = InventoryComponent.Get())
	{
		Comp->OnNewItemAdded.AddLambda([this](const FItemDefinition& ItemDef, const FGridInfo& GridInfo)
		{
			CreateNewItemDelegate.ExecuteIfBound(ItemDef, GridInfo);
		});
	}
}

bool UGSInventoryMenuWidgetController::FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	return FindNewSpaceDelegate.Execute(ItemSize, OutGridInfo);
}

void UGSInventoryMenuWidgetController::CallOnGridItemProxyStatusChanged(bool bProxyExists, const FItemSize& ProxySize)
{
	OnItemProxyStatusChanged.Broadcast(bProxyExists, ProxySize);
}

void UGSInventoryMenuWidgetController::TryRelocateItemGrid(int32 GridIndex)
{
	if (GridItemRef.IsValid())
	{
		RelocateGridItemDelegate.ExecuteIfBound(GridIndex, GridItemRef.Get());
		GridItemRef->GetItemProxy()->RemoveProxy();
	}
	
}

void UGSInventoryMenuWidgetController::SetProxyGridItem(UGSGridItem* GridItem)
{
	GridItemRef = GridItem;
}
