// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSInventoryGrid.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "UI/Widgets/Inventory/GSGridItem.h"
#include "UI/Widgets/Inventory/GSGridSlot.h"


void UGSInventoryGrid::NativePreConstruct()
{
	Super::NativePreConstruct();

	ConstructGrid();
}

void UGSInventoryGrid::ConstructGrid()
{
	if (!GridSlotClass)
	{
		return;
	}
	
	GridSlots.Reserve(RowsNum * ColumnsNum);

	for (int32 i = 0; i < RowsNum; ++i)
	{
		for (int32 j = 0; j < ColumnsNum; ++j)
		{
			if (UGSGridSlot* SlotWidget = CreateWidget<UGSGridSlot>(this, GridSlotClass))
			{
				GridSlots.Add(SlotWidget);
				SlotWidget->SetPosition(i, j);
				SlotWidget->CheckAllItemPositionsDelegate.BindUObject(this, &UGSInventoryGrid::CheckRelocationPositions);
				SlotWidget->ClearAllItemsPositionsDelegate.BindUObject(this, &UGSInventoryGrid::ClearRelocationPositions);
				UCanvasPanelSlot* GridSlot = GridPanel->AddChildToCanvas(SlotWidget);
				GridSlot->SetPosition(FVector2D(j * SlotSize, i * SlotSize));
				GridSlot->SetSize(FVector2D( SlotSize, SlotSize));		
			}			
		}
	}
}

void UGSInventoryGrid::AddItem(const FItemDefinition& ItemDef, const TArray<FGridPosition>& Positions)
{
	if (!GridItemClass)
	{
		return;
	}
	
	if (UGSGridItem* GridItem = CreateWidget<UGSGridItem>(this, GridItemClass))
	{
		GridItem->ConstructItem(ItemDef, SlotSize);
		UCanvasPanelSlot* Item = GridPanel->AddChildToCanvas(GridItem);
		Item->SetPosition(FVector2D(Positions[0].ColumnsIndex * SlotSize, Positions[0].RowsIndex * SlotSize));
		Item->SetSize(FVector2D(SlotSize * GridItem->GetGridSize().RowSize, SlotSize * GridItem->GetGridSize().ColumnSize));
		AssignItemToSlots(GridItem, Positions);
		GridItems.Add(GridItem);
	}			
}

bool UGSInventoryGrid::FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	TArray<FGridPosition> Positions;
	Positions.Reserve(ItemSize.RowSize * ItemSize.ColumnSize);
	
	for (int32 i = 0; i < GridSlots.Num(); ++i)
	{
		if (GridSlots[i] && !GridSlots[i]->IsOccupied())
		{
			Positions.Reset();
			const auto [RowIndex, ColumnIndex] = GridSlots[i]->GetPosition();
			if (CheckAllPositions(RowIndex, ColumnIndex, ItemSize, Positions))
			{
				OutGridInfo.Positions = Positions;
				return true;
			}
		}
	}
	return false;
}

void UGSInventoryGrid::AssignItemToSlots(UGSGridItem* Item, const TArray<FGridPosition>& Positions)
{
	for (const auto& Position : Positions)
	{
		if (UGSGridSlot* GridSlot = GetGridSlotAtPosition(Position.RowsIndex, Position.ColumnsIndex))
		{
			GridSlot->SetOccupiedStatus(true);
		}
	}
}

bool UGSInventoryGrid::CheckAllPositions(int32 RowIndex, int32 ColumnIndex, const FItemSize& ItemSize, TArray<FGridPosition>& Positions)
{
	const int32 EndRow = RowIndex + ItemSize.RowSize;
	const int32 EndColumn = ColumnIndex + ItemSize.ColumnSize;
	if (EndRow > RowsNum || EndColumn > ColumnsNum)
	{
		return false;
	}
		
	for (int32 i = RowIndex; i < EndRow; ++i)
	{
		for (int32 j = ColumnIndex; j < EndColumn; ++j)
		{
			if (const UGSGridSlot* GridSlot = GetGridSlotAtPosition(i, j))
			{
				if (GridSlot->IsOccupied())
				{
					return false;
				}
				Positions.Add({i, j});
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

UGSGridSlot* UGSInventoryGrid::GetGridSlotAtPosition(int32 RowIndex, int32 ColumnIndex)
{
	const int32 Index = ColumnIndex + (RowIndex * ColumnsNum);	
	if (GridSlots.IsValidIndex(Index))
	{
		return GridSlots[Index];
	}
	return nullptr;
}

TArray<FGridPosition> UGSInventoryGrid::CheckRelocationPositions(const FGridPosition& Position, const FItemSize& ProxySize)
{
	TArray<UGSGridSlot*> Slots;
	TArray<FGridPosition> Positions;
	Slots.Reserve(ProxySize.RowSize * ProxySize.ColumnSize);
	Positions.Reserve(ProxySize.RowSize * ProxySize.ColumnSize);
	
	bool RelocationAllowed = true;
	for (int32 i = 0; i < ProxySize.RowSize; ++i)
	{
		for (int32 j = 0; j < ProxySize.ColumnSize; ++j)
		{
			const int32 RowPosition = Position.RowsIndex + i < RowsNum ? Position.RowsIndex + i : Position.RowsIndex + i - 2;
			const int32 ColumnPosition = Position.ColumnsIndex + j < ColumnsNum ? Position.ColumnsIndex + j : Position.ColumnsIndex + j - 2;
			if (UGSGridSlot* GridSlot = GetGridSlotAtPosition(RowPosition, ColumnPosition ))
			{
				Positions.Add(FGridPosition(RowPosition, ColumnPosition));
				if (GridSlot->IsOccupied())
				{
					RelocationAllowed = false;
				}
				Slots.Add(GridSlot);
			}
		}
	}

	for (const auto GridSlot : Slots)
	{
		GridSlot->SetHoveredColor(RelocationAllowed);
	}

	return Positions;
}

void UGSInventoryGrid::ClearRelocationPositions(const TArray<FGridPosition>& Positions)
{
	for (const auto& Position : Positions)
	{
		if (UGSGridSlot* GridSlot = GetGridSlotAtPosition(Position.RowsIndex, Position.ColumnsIndex))
		{
			GridSlot->ClearSlot();
		}
	}
}
