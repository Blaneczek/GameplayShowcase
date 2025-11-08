// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridSlot.h"

#include "Components/Border.h"

void UGSGridSlot::OccupieSlot(bool bIsOccupied)
{
	if (bIsOccupied)
	{
		GridSlot->SetBrushColor(FLinearColor::Gray);
	}	
}
