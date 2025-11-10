// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridSlot.h"
#include "Components/Border.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"

void UGSGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists && GridSlot && CheckAllItemPositionsDelegate.IsBound())
	{
		PositionsToClear = CheckAllItemPositionsDelegate.Execute(Position, ProxySize);
	}
}

void UGSGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists)
	{
		ClearAllItemsPositionsDelegate.ExecuteIfBound(PositionsToClear);
	}	
}

void UGSGridSlot::SetOccupiedStatus(bool bIsOccupied)
{
	bOccupied = bIsOccupied;
}

void UGSGridSlot::SetItemProxyStatus(bool bProxyExists, const FItemSize& inProxySize)
{
	bItemProxyExists = bProxyExists;
	ProxySize = inProxySize;
	
	if (!bItemProxyExists)
	{
		ClearSlot();
	}
}

void UGSGridSlot::SetHoveredColor(bool bCanRelocateItem)
{
	if (GridSlot)
	{
		bCanRelocateItem ? GridSlot->SetBrushColor(RelocationAllowedColor) : GridSlot->SetBrushColor(RelocationForbiddenColor);
	}
}

void UGSGridSlot::ClearSlot()
{
	if (GridSlot)
	{
		GridSlot->SetBrushColor(DefaultColor);
	}
}

void UGSGridSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGSInventoryMenuWidgetController* Controller = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		Controller->OnItemProxyStatusChanged.AddUObject(this, &UGSGridSlot::SetItemProxyStatus);
	}
}

