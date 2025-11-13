// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridItem.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "UI/Controllers/GSOverlayWidgetController.h"
#include "UI/Widgets/Inventory/GSGridItemProxy.h"

void UGSGridItem::ConstructGridItem(int32 InInventoryGridIndex, const FItemInstance& Item, float inSlotSize, const TArray<FGridPosition>& Positions)
{
	const FItemDefinition& Def = Item.GetItemDefinition();
	InventoryGridIndex = InInventoryGridIndex;
	ItemID = Item.GetInstanceID();
	ItemType = Def.Type;
	GridPositions = Positions;
	SlotSize = inSlotSize;
	
	SetGridItemSize(Def.GetFragmentByType<FGridFragment>());
	SetGridItemIcon(Def.GetFragmentByType<FImageFragment>());

	BindToInventoryController();
}

void UGSGridItem::SetGridItemSize(const FGridFragment* GridFragment)
{
	if (GridFragment)
	{
		GridItemSize = GridFragment->GetGridSize();
	}
	// Item occupies 1 slot
	else
	{
		GridItemSize = FItemSize(1,1);
	}
}

void UGSGridItem::SetGridItemIcon(const FImageFragment* ImageFragment)
{
	if (ImageFragment&& ImageFragment->GetIcon())
	{
		ItemIcon->SetBrushFromTexture(ImageFragment->GetIcon(), false);
	}	
	else if (AltIcon)
	{
		ItemIcon->SetBrushFromTexture(AltIcon, false);
	}
}

void UGSGridItem::CreateItemProxy()
{
	if (!ItemProxyClass)
	{
		return;
	}
	
	ItemProxy = CreateWidget<UGSGridItemProxy>(this, ItemProxyClass);
	if (ItemProxy)
	{
		if (const UGSOverlayWidgetController* Controller = UGSBlueprintFunctionLibrary::GetOverlayWidgetController(this))
		{
			UCanvasPanelSlot* ProxySlot = Controller->GetCanvasPanelRef()->AddChildToCanvas(ItemProxy);
			const FVector2D WidgetSize = FVector2D(SlotSize * GridItemSize.ColumnSize, SlotSize * GridItemSize.RowSize);
			UTexture2D* Icon = Cast<UTexture2D>(ItemIcon->GetBrush().GetResourceObject());
			ItemProxy->InitProxy(Icon,this, ProxySlot, Controller->CanvasRef, WidgetSize, GridItemSize);

			if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
			{
				InvController->CallOnGridItemProxyStatusChanged(true, GridItemSize);
				InvController->SetGridItemRef(this);
			}
		}
	}
}

FReply UGSGridItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CreateItemProxy();
        return FReply::Handled();
	}
	
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
		{
			InvController->SetGridItemRef(this);
			bIsEquipped ? InvController->TryUnequipGridItem() : InvController->TryActivateItemAction(ItemID);
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UGSGridItem::BindToInventoryController()
{
	// If any proxy exists, don't allow to click on a GridItem 
	if (UGSInventoryMenuWidgetController* InvController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		CachedInventoryController = InvController;
		InvController->OnItemProxyStatusChanged.AddLambda([this](bool bProxyExists, const FItemSize& Size)
		{
			bProxyExists ? SetVisibility(ESlateVisibility::HitTestInvisible): SetVisibility(ESlateVisibility::Visible);
		});
	}
}

void UGSGridItem::RemoveProxy()
{
	if (ItemProxy)
	{
		ItemProxy->RemoveProxy();
	}
}

void UGSGridItem::ResetGridPositions()
{
	GridPositions.Reset();
}
