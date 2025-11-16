// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridSlot.h"
#include "Components/Border.h"
#include "GSBlueprintFunctionLibrary.h"
#include "UI/Widgets/Inventory/GSGridItem.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"

void UGSGridSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGSInventoryMenuWidgetController* Controller = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		Controller->OnItemProxyStatusChanged.AddUObject(this, &UGSGridSlot::SetGridItemProxyStatus);
		CachedInventoryController = Controller;
	}
}

void UGSGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bItemProxyExists && CheckAllGridItemPositionsDelegate.IsBound())
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
	if (bItemProxyExists && bRelocationAllowed)
	{
		UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get();
		if (!InvController)
		{
			return FReply::Handled();
		}
		
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (!bCanAddToStack)
			{
				InvController->RelocateGridItem(InventoryGridIndex);				
			}
			else
			{
				if (const UGSGridItem* OccupyingGridItem = OccupyingGridItemRef.Get())
				{
					InvController->TryAddToStack(OccupyingGridItem->GetItemID());
				}				
			}				
		}
		else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			InvController->RemoveActiveGridItemProxy();
		}
	}
	return FReply::Handled();
}

void UGSGridSlot::SetOccupancyStatus(bool bIsOccupied, UGSGridItem* OccupyingGridItem)
{
	bOccupied = bIsOccupied;
	bOccupied ? OccupyingGridItemRef = OccupyingGridItem : OccupyingGridItemRef = nullptr;
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

void UGSGridSlot::SetRelocationStatus(bool bCanRelocateItem, bool InCanAddToStack)
{
	bRelocationAllowed = bCanRelocateItem;
	bCanAddToStack = InCanAddToStack;
	bCanRelocateItem ? GridSlot->SetBrushColor(RelocationAllowedColor) : GridSlot->SetBrushColor(RelocationForbiddenColor);
}

void UGSGridSlot::ClearSlot()
{
	GridSlot->SetBrushColor(DefaultColor);
}



