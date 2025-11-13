// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridSlot.h"
#include "Components/Border.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"

void UGSGridSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGSInventoryMenuWidgetController* Controller = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		Controller->OnItemProxyStatusChanged.AddUObject(this, &UGSGridSlot::SetGridItemProxyStatus);
	}
}

void UGSGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists && GridSlot && CheckAllGridItemPositionsDelegate.IsBound())
	{
		CheckAllGridItemPositionsDelegate.Execute(Position, ProxySize);
	}
}

void UGSGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists)
	{
		ClearAllGridItemsPositionsDelegate.ExecuteIfBound();
	}	
}

FReply UGSGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists && bRelocationAllowed && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (UGSInventoryMenuWidgetController* Controller = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
		{
			Controller->RelocateGridItem(InventoryGridIndex);
		}
	}
	return FReply::Handled();
}

void UGSGridSlot::SetOccupancyStatus(bool bIsOccupied)
{
	bOccupied = bIsOccupied;
}

void UGSGridSlot::SetGridItemProxyStatus(bool bProxyExists, const FItemSize& inProxySize)
{
	bItemProxyExists = bProxyExists;
	ProxySize = inProxySize;
	
	if (!bItemProxyExists)
	{
		ClearSlot();
	}
}

void UGSGridSlot::SetRelocationStatus(bool bCanRelocateItem)
{
	bRelocationAllowed = bCanRelocateItem;
	bCanRelocateItem ? GridSlot->SetBrushColor(RelocationAllowedColor) : GridSlot->SetBrushColor(RelocationForbiddenColor);
}

void UGSGridSlot::ClearSlot()
{
	GridSlot->SetBrushColor(DefaultColor);
}



