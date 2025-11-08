// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSInventoryGrid.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
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
	
	GridSlots.Reserve(Rows * Columns);

	for (int32 i = 0; i < Rows; ++i)
	{
		for (int32 j = 0; j < Columns; ++j)
		{
			if (UGSGridSlot* SlotWidget = CreateWidget<UGSGridSlot>(this, GridSlotClass))
			{
				GridSlots.Add(SlotWidget);
				UUniformGridSlot* GridSlot = GridPanel->AddChildToUniformGrid(SlotWidget, i, j);
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);				
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
		GridItem->ConstructItem(ItemDef);
		AssignItemToSlots(GridItem, Positions);
		GridItems.Add(GridItem);
	}			
}

bool UGSInventoryGrid::FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	// TODO:

	// Debug
	TArray<FGridPosition> Positions{ {0,1}, {0,2}, {1,1}, {1,2}};
	OutGridInfo.Positions = Positions;
	return true;
}

void UGSInventoryGrid::AssignItemToSlots(UGSGridItem* Item, const TArray<FGridPosition>& Positions)
{
	for (const auto& Position : Positions)
	{
		GridSlots[Position.ColumnsIndex + (Position.RowsIndex * Columns)]->OccupieSlot(true);
	}
}
