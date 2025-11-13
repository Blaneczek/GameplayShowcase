// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/GSInventoryMenuWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Widgets/Inventory/GSGridItem.h"
#include "UI/Widgets/Inventory/GSGridSlot.h"
#include "UI/Widgets/Inventory/GSInventoryGrid.h"


void UGSInventoryMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( UGSInventoryMenuWidgetController* InvController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		InvController->CreateNewItemDelegate.BindUObject(this, &UGSInventoryMenuWidget::AddGridItem);
		InvController->FindNewSpaceDelegate.BindUObject(this, &UGSInventoryMenuWidget::FindFreeSpaceForItem);
		InvController->RelocateGridItemDelegate.BindUObject(this, &UGSInventoryMenuWidget::RelocateGridItem);
		InvController->CanEquipGridItemDelegate.BindUObject(this, &UGSInventoryMenuWidget::CanEquipItem);
		InvController->EquipGridItemDelegate.BindUObject(this, &UGSInventoryMenuWidget::EquipGridItem);
		InvController->FindEquipGridSlotDelegate.BindUObject(this, &UGSInventoryMenuWidget::FindEquipGridSlot);
		InvController->TryUnequipGridItemDelegate.BindUObject(this, &UGSInventoryMenuWidget::TryUnequipGridItem);
	}
}

bool UGSInventoryMenuWidget::FindFreeSpaceForItem(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	for (const auto& Grid : InventoryGrids)
	{
		if (Grid.Value->FindFreeSpaceForItem(ItemSize, OutGridInfo))
		{
			OutGridInfo.InventoryGridIndex = Grid.Key;
			return true;		
		}
	}
	return false;
}

FReply UGSInventoryMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UGSInventoryMenuWidget::RegisterInventoryGrid(UGSInventoryGrid* NewInventoryGrid)
{
	if (!NewInventoryGrid)
	{
		return;
	}
	
	if (!InventoryGrids.Contains(NewInventoryGrid->GetInventoryGridIndex()))
	{
		InventoryGrids.Add(NewInventoryGrid->GetInventoryGridIndex(), NewInventoryGrid);
	}
}

void UGSInventoryMenuWidget::RegisterEquipGridSlots(TMap<FGameplayTag, UGSGridSlot*> InEquipSlots)
{
	EquipSlots = InEquipSlots;
}

void UGSInventoryMenuWidget::AddGridItem(const FItemInstance& Item, const FGridInfo& GridInfo)
{
	if (UGSInventoryGrid* Grid = *InventoryGrids.Find(GridInfo.InventoryGridIndex))
	{
		Grid->AddNewGridItem(Item, GridInfo.Positions);
	}
}

void UGSInventoryMenuWidget::RelocateGridItem(UGSGridItem* GridItem, int32 InventoryGridIndex)
{
	UGSInventoryGrid* Grid = *InventoryGrids.Find(InventoryGridIndex);
	UGSInventoryGrid* PreviousGrid = *InventoryGrids.Find(GridItem->GetInventoryGridIndex());
	if (!Grid || !PreviousGrid)
	{
		return;
	}
	PreviousGrid->SetSlotsOccupancy(GridItem->GetGridPositions(), false); 
	PreviousGrid->GetGridPanel()->RemoveChild(GridItem);
	RelocateGridItem(GridItem, InventoryGridIndex, Grid->MoveProxyPositions());
}

void UGSInventoryMenuWidget::RelocateGridItem(UGSGridItem* GridItem, int32 InventoryGridIndex, TArray<FGridPosition>&& Positions)
{
	UGSInventoryGrid* Grid = *InventoryGrids.Find(InventoryGridIndex);
	if (!Grid)
	{
		return;
	}
	
	if (GridItem->bIsEquipped)
	{
		EquipmentPanel->RemoveChild(GridItem);
		GridItem->bIsEquipped = false;	
	}
	UCanvasPanelSlot* CanvasSlot = Grid->GetGridPanel()->AddChildToCanvas(GridItem);
    CanvasSlot->SetSize(GridItem->GetCanvasSlotSize());
    GridItem->SetCanvasSlot(CanvasSlot);
    Grid->RelocateGridItem(GridItem, MoveTemp(Positions));
}

bool UGSInventoryMenuWidget::CanEquipItem(const FGameplayTag& ItemType, UGSGridSlot* ClickedSlot, UGSGridSlot*& OutEquipSlot)
{
	if (const FGameplayTag* SlotTag = EquipSlots.FindKey(ClickedSlot))
	{
		if (SlotTag->RequestDirectParent().MatchesTagExact(ItemType.RequestDirectParent()))
		{
			OutEquipSlot = *EquipSlots.Find(ItemType);
			return true;
		}
	}
	return false;
}

bool UGSInventoryMenuWidget::TryUnequipGridItem(UGSGridItem* UnequippedGridItem)
{
	if (!UnequippedGridItem)
	{
		return false;
	}
	
	const FItemSize ItemSize = UnequippedGridItem->GetGridItemSize();
	FGridInfo GridInfo;
	if (FindFreeSpaceForItem(ItemSize, GridInfo))
	{
		RelocateGridItem(UnequippedGridItem, GridInfo.InventoryGridIndex, MoveTemp(GridInfo.Positions));
		return true;
	}
	return false;
}

void UGSInventoryMenuWidget::EquipGridItem(UGSGridItem* GridItem, UGSGridSlot* EquipSlot)
{
	if (!GridItem || !EquipSlot)
	{
		return;
	}
	
	if (UGSInventoryGrid* Grid = *InventoryGrids.Find(GridItem->GetInventoryGridIndex()))
	{
		GridItem->bIsEquipped = true;
		EquipSlot->SetOccupancyStatus(true);
		Grid->RemoveGridItem(GridItem);
		
		UCanvasPanelSlot* CanvasItemSlot = EquipmentPanel->AddChildToCanvas(GridItem);
		const UCanvasPanelSlot* CanvasEquipSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(EquipSlot);

		CanvasItemSlot->SetPosition(CanvasEquipSlot->GetPosition());
		CanvasItemSlot->SetSize(GridItem->GetCanvasSlotSize());
		GridItem->SetCanvasSlot(CanvasItemSlot);		
	}
}

UGSGridSlot* UGSInventoryMenuWidget::FindEquipGridSlot(const FGameplayTag& EquipType)
{
	return *EquipSlots.Find(EquipType);
}
