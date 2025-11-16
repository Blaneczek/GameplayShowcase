// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSInventoryGrid.h"

#include "GSBlueprintFunctionLibrary.h"
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

void UGSInventoryGrid::NativeConstruct()
{
	Super::NativeConstruct();

	CachedInventoryController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this);
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
				SlotWidget->SetInventoryGridIndex(InventoryGridIndex);
				SlotWidget->SetPosition(i, j);
				
				UCanvasPanelSlot* GridSlot = GridPanel->AddChildToCanvas(SlotWidget);
				GridSlot->SetPosition(FVector2D(j * SlotSize, i * SlotSize));
				GridSlot->SetSize(FVector2D( SlotSize, SlotSize));		
				
				SlotWidget->CheckAllGridItemPositionsDelegate.BindUObject(this, &UGSInventoryGrid::SetProxyPositionsStatus);
                SlotWidget->ClearAllGridItemsPositionsDelegate.BindUObject(this, &UGSInventoryGrid::ClearProxyPositions);
			}			
		}
	}
}

void UGSInventoryGrid::AddNewGridItem(const FItemInstance* Item, const TArray<FGridPosition>& Positions)
{
	if (!GridItemClass)
	{
		return;
	}
	
	if (UGSGridItem* GridItem = CreateWidget<UGSGridItem>(this, GridItemClass))
	{
		GridItem->OnGridItemRemoved.BindUObject(this, &UGSInventoryGrid::RemoveGridItemPermanently);
		GridItem->ConstructGridItem(InventoryGridIndex, Item, SlotSize, Positions);
		UCanvasPanelSlot* CanvasSlot = GridPanel->AddChildToCanvas(GridItem);
		CanvasSlot->SetPosition(FVector2D(Positions[0].ColumnsIndex * SlotSize, Positions[0].RowsIndex * SlotSize));
		CanvasSlot->SetSize(FVector2D(SlotSize * GridItem->GetGridItemSize().ColumnSize, SlotSize * GridItem->GetGridItemSize().RowSize));
		GridItem->SetCanvasSlot(CanvasSlot);
		SetSlotsOccupancy(Positions, true, GridItem);

		GridItems.Add(GridItem);
	}			
}

bool UGSInventoryGrid::FindFreeSpaceForItem(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
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

void UGSInventoryGrid::RelocateGridItem(UGSGridItem* GridItem, TArray<FGridPosition>&& Positions)
{
	GridItem->ResetGridPositions();
	SetSlotsOccupancy(Positions, true, GridItem);
	
	const FGridPosition FirstPosition = GetFirstGridPosition(Positions);
	GridItem->GetCanvasSlot()->SetPosition(FVector2D(FirstPosition.ColumnsIndex * SlotSize, FirstPosition.RowsIndex * SlotSize));
	
	GridItem->SetGridPositions(MoveTemp(Positions));	
}

void UGSInventoryGrid::SetSlotsOccupancy(const TArray<FGridPosition>& Positions, bool bOccupied, UGSGridItem* OccupyingGridItem)
{
	if (Positions.IsEmpty())
	{
		return;
	}
	
	for (const auto& Position : Positions)
	{
		if (UGSGridSlot* GridSlot = GetGridSlotAtPosition(Position.RowsIndex, Position.ColumnsIndex))
		{
			GridSlot->SetOccupancyStatus(bOccupied, OccupyingGridItem);
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

void UGSInventoryGrid::SetProxyPositionsStatus(const FGridPosition& Position, const FItemSize& ProxySize)
{
	TArray<UGSGridSlot*> Slots;	
	Slots.Reserve(ProxySize.RowSize * ProxySize.ColumnSize);
	ProxyPositions.Reset(ProxySize.RowSize * ProxySize.ColumnSize);

	const bool bCanAddToStack = CheckIfCanAddToStack(Position);
	
	bool bIsRelocationAllowed = true;
	for (int32 i = 0; i < ProxySize.RowSize; ++i)
	{
		for (int32 j = 0; j < ProxySize.ColumnSize; ++j)
		{
			const int32 RowPosition = Position.RowsIndex + i < RowsNum ? Position.RowsIndex + i : Position.RowsIndex + i - 2;
			const int32 ColumnPosition = Position.ColumnsIndex + j < ColumnsNum ? Position.ColumnsIndex + j : Position.ColumnsIndex + j - 2;
			if (UGSGridSlot* GridSlot = GetGridSlotAtPosition(RowPosition, ColumnPosition))
			{
				ProxyPositions.Add(FGridPosition(RowPosition, ColumnPosition));
				if (GridSlot->IsOccupied())
				{
					bIsRelocationAllowed = bCanAddToStack;
				}
				Slots.Add(GridSlot);
			}
		}
	}

	for (const auto GridSlot : Slots)
	{
		GridSlot->SetRelocationStatus(bIsRelocationAllowed, bCanAddToStack);
	}
}

void UGSInventoryGrid::ClearProxyPositions()
{
	for (const auto& Position : ProxyPositions)
	{
		if (UGSGridSlot* GridSlot = GetGridSlotAtPosition(Position.RowsIndex, Position.ColumnsIndex))
		{
			GridSlot->ClearSlot();
		}
	}
}

FGridPosition UGSInventoryGrid::GetFirstGridPosition(const TArray<FGridPosition>& Positions)
{
	if (Positions.IsEmpty())
	{
		return FGridPosition();
	}
	const FGridPosition* Position = Algo::MinElement(Positions);
	return *Position;
}

bool UGSInventoryGrid::CheckIfCanAddToStack(const FGridPosition& Position)
{
	const UGSGridSlot* GridSlot = GetGridSlotAtPosition(Position.RowsIndex, Position.ColumnsIndex);
	if (!GridSlot)
	{
		return false;
	}

	if (!GridSlot->IsOccupied())
	{
		return false;
	}
	if (const UGSGridItem* GridItem = GridSlot->GetOccupyingGridItem())
	{
		if (UGSInventoryMenuWidgetController* InvController = CachedInventoryController.Get())
		{
			return InvController->CheckIfCanAddToStack(GridItem->GetItemID());
		}
	}
	return false;
}

void UGSInventoryGrid::RemoveGridItemTemporary(UGSGridItem* GridItem)
{
	SetSlotsOccupancy(GridItem->GetGridPositions(), false);
	GridPanel->RemoveChild(GridItem);
	GridItems.Remove(GridItem);
}

void UGSInventoryGrid::RemoveGridItemPermanently(UGSGridItem* GridItem)
{
	SetSlotsOccupancy(GridItem->GetGridPositions(), false);
	GridPanel->RemoveChild(GridItem);
	GridItems.Remove(GridItem);
	GridItem->RemoveFromParent();
}




