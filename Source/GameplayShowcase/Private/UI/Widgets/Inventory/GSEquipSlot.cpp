// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSEquipSlot.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"


FReply UGSEquipSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (UGSInventoryMenuWidgetController* Controller = CachedInventoryController.Get())
		{
			Controller->TryEquipGridItem(this);
		}		
	}
	return FReply::Handled();
}
