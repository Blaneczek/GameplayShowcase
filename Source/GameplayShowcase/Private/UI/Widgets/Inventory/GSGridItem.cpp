// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridItem.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "UI/Controllers/GSOverlayWidgetController.h"
#include "UI/Widgets/Inventory/GSGridItemProxy.h"

void UGSGridItem::ConstructItem(const FItemDefinition& Def, float inSlotSize, const TArray<FGridPosition>& Positions)
{
	GridPositions = Positions;
	SlotSize = inSlotSize;
	SetItemSize(Def);
	SetItemImage(Def);

	if (UGSInventoryMenuWidgetController* InvController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		InvController->OnItemProxyStatusChanged.AddLambda([this](bool bProxyExists, const FItemSize& Size)
		{
			bProxyExists ? SetVisibility(ESlateVisibility::HitTestInvisible): SetVisibility(ESlateVisibility::Visible);
		});
	}
}

void UGSGridItem::SetItemSize(const FItemDefinition& Def)
{
	if (const FGridFragment* GridFrag = Def.GetFragmentByType<FGridFragment>())
	{
		ItemSize = GridFrag->GetGridSize();
	}
	// Item occupies 1 slot
	else
	{
		ItemSize = FItemSize(1,1);
	}
}

void UGSGridItem::SetItemImage(const FItemDefinition& Def)
{
	const FImageFragment* ImageFrag = Def.GetFragmentByType<FImageFragment>();
	if (ImageFrag && ImageFrag->GetIcon())
	{
		ItemIcon->SetBrushFromTexture(ImageFrag->GetIcon(), false);
		return;
	}
	
	if (AltIcon)
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
		if (UGSOverlayWidgetController* Controller = UGSBlueprintFunctionLibrary::GetOverlayWidgetController(this))
		{
			UCanvasPanelSlot* ProxySlot = Controller->CanvasRef->AddChildToCanvas(ItemProxy);
			const FVector2D WidgetSize = FVector2D(SlotSize * ItemSize.ColumnSize, SlotSize * ItemSize.RowSize);
			UTexture2D* Icon = Cast<UTexture2D>(ItemIcon->GetBrush().GetResourceObject());
			ItemProxy->InitProxy(Icon,this, ProxySlot, Controller->CanvasRef, WidgetSize, ItemSize);

			if (UGSInventoryMenuWidgetController* InvController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
			{
				InvController->CallOnGridItemProxyStatusChanged(true, ItemSize);
				InvController->SetProxyGridItem(this);
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
	return FReply::Unhandled();
}
