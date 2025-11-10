// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "Systems/Inventory/GSInventoryComponent.h"

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
			CreateNewItemDelegate.Broadcast(ItemDef, GridInfo);
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
