// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridItem.h"

#include "GSBlueprintFunctionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "UI/Widgets/Inventory/GSGridItemProxy.h"
#include "UI/Widgets/Inventory/GSItemTooltip.h"

void UGSGridItem::ConstructGridItem(int32 InInventoryGridIndex, const FItemInstance* Item, float inSlotSize, const TArray<FGridPosition>& Positions)
{
	ItemID = Item->GetInstanceID();
	const FItemDefinition& Def = Item->GetItemDefinition();
	if (Def.GetFragmentByType<FStackableFragment>())
	{
		UpdateStackCount(ItemID, Item->GetStackCount());
		StackCountText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}
	
	InventoryGridIndex = InInventoryGridIndex;
	ItemType = Def.Type;
	GridPositions = Positions;
	SlotSize = inSlotSize;
	CachedMainCanvasPanel = UGSBlueprintFunctionLibrary::GetOverlayCanvasPanelRef(this);
	CachedInventoryController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this);
	
	SetGridItemSize(Def.GetFragmentByType<FGridFragment>());
	SetGridItemIcon(Def.GetFragmentByType<FImageFragment>());

	InitItemTooltip(Item);
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
	UCanvasPanel* MainCanvasPanel = CachedMainCanvasPanel.Get();
	if (!ItemProxy || !MainCanvasPanel)
	{
		return;
	}

	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	UCanvasPanelSlot* ProxySlot = MainCanvasPanel->AddChildToCanvas(ItemProxy);
	const FVector2D WidgetSize = FVector2D(SlotSize * GridItemSize.ColumnSize, SlotSize * GridItemSize.RowSize);
	UTexture2D* Icon = Cast<UTexture2D>(ItemIcon->GetBrush().GetResourceObject());
	ItemProxy->InitProxy(Icon,this, ProxySlot, CachedMainCanvasPanel, WidgetSize, GridItemSize);

	if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
	{
		InvController->CallOnGridItemProxyStatusChanged(true, GridItemSize);
		InvController->SetGridItemRef(this);
	}
}

void UGSGridItem::SetToRemove(const FGuid& InstanceID)
{
	if (InstanceID == ItemID)
	{
		if (ItemTooltip)
		{
			ItemTooltip->RemoveFromParent();
		}
		if (ItemProxy)
		{
			ItemProxy->RemoveProxy();
		}
		OnGridItemRemoved.ExecuteIfBound(this);
	}	
}

FReply UGSGridItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
		{
			if (InvController->IsProxyExists())
			{
				InvController->TryAddToStack(ItemID);
				return FReply::Handled();
			}		
		}
		CreateItemProxy();
	}
	return FReply::Handled();
}

FReply UGSGridItem::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
		{
			InvController->SetGridItemRef(this);
			bIsEquipped ? InvController->TryUnequipGridItem() : InvController->TryActivateItemAction(ItemID);
		}
	}
	return FReply::Handled();
}

void UGSGridItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UGSGridItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (ItemTooltip)
	{
		ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

FReply UGSGridItem::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ItemTooltip)
	{
		const FVector2D WidgetSize = ItemTooltip->GetDesiredSize();
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		// Tooltip above mouse
		FVector2D NewPosition = FVector2D(MousePosition.X - WidgetSize.X * 0.5f, MousePosition.Y - WidgetSize.Y - 5.f);	
		// Prevent going outside screen
		NewPosition.X = FMath::Clamp(NewPosition.X, 0.f, MainCanvasSize.X - WidgetSize.X);
		NewPosition.Y = FMath::Clamp(NewPosition.Y, 0.f, MainCanvasSize.Y - WidgetSize.Y);	
		TooltipCanvasSlotRef->SetPosition(NewPosition);
		
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UGSGridItem::InitItemTooltip(const FItemInstance* Item)
{
	UCanvasPanel* MainCanvasPanel = CachedMainCanvasPanel.Get();
	if (!ItemTooltipClass || !MainCanvasPanel)
	{
		return;
	}

	if (UGSItemTooltip* Tooltip = CreateWidget<UGSItemTooltip>(this, ItemTooltipClass))
	{
		ItemTooltip = Tooltip;
		Tooltip->SetVisibility(ESlateVisibility::Collapsed);
		
		const FItemDefinition& Def = Item->GetItemDefinition();
		TArray<const FWidgetFragment*> Fragments = Def.GetAllFragmentsByType<FWidgetFragment>();
		
		Def.AdaptItemNameToWidget(Tooltip);
		for (const auto& Fragment : Fragments)
		{
			Fragment->AdaptToWidget(Tooltip);
		}
		Def.AdaptItemTypeToWidget(Tooltip);
		
		TooltipCanvasSlotRef = MainCanvasPanel->AddChildToCanvas(Tooltip);
		TooltipCanvasSlotRef->SetZOrder(100);
		TooltipCanvasSlotRef->SetAutoSize(true);
		MainCanvasSize = MainCanvasPanel->GetCachedGeometry().GetLocalSize();
	}
}

void UGSGridItem::BindToInventoryController()
{
	if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
	{
		InvController->OnItemProxyStatusChanged.AddLambda([this](bool bProxyExists, const FItemSize& ProxySize)
		{
			bProxyExists ? SetVisibility(ESlateVisibility::HitTestInvisible) : SetVisibility(ESlateVisibility::Visible);
		});
		InvController->OnStackCountChanged.AddUObject(this, &UGSGridItem::UpdateStackCount);
		InvController->OnItemRemoved.AddUObject(this, &UGSGridItem::SetToRemove);
	}
}

void UGSGridItem::UpdateStackCount(const FGuid& InstanceID, int32 NewStackCount)
{
	if (InstanceID == ItemID)
	{
		StackCountText->SetText(FText::FromString(FString::FromInt(NewStackCount)));
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
