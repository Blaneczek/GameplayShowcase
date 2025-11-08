// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/GSInventoryMenuWidget.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Widgets/Inventory/GSInventoryGrid.h"

void UGSInventoryMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InventoryController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this);
	if (InventoryController)
	{
		InventoryController->CreateNewItemDelegate.AddUObject(this, &UGSInventoryMenuWidget::AddItem);
		InventoryController->FindNewSpaceDelegate.BindUObject(this, &UGSInventoryMenuWidget::FindFreeSpaceForItem);
	}
}

bool UGSInventoryMenuWidget::FindFreeSpaceForItem(const FItemSize& ItemSize, FGridInfo& OutGridInfo)
{
	for (const auto& Grid : InventoryGrids)
	{
		if (Grid.Value->FindFreeSpace(ItemSize, OutGridInfo))
		{
			OutGridInfo.GridIndex = Grid.Key;
			return true;
		}
	}
	return false;
}

void UGSInventoryMenuWidget::RegisterInventoryGrid(int32 Index, UGSInventoryGrid* NewInventoryGrid)
{
	if (!InventoryGrids.Contains(Index))
	{
		InventoryGrids.Add(Index, NewInventoryGrid);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGSInventoryMenuWidget::RegisterInventoryGrid | Grid with Index [%d] already exists"), Index);
	}
}

void UGSInventoryMenuWidget::AddItem(const FItemDefinition& ItemDef, const FGridInfo& GridInfo)
{
	if (!InventoryGrids.Contains(GridInfo.GridIndex))
	{
		return;
	}
	
	if (UGSInventoryGrid* Grid = InventoryGrids[GridInfo.GridIndex])
	{
		Grid->AddItem(ItemDef, GridInfo.Positions);
	}
}
