// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSItemDrag.h"

UGSItemDrag::UGSItemDrag()
{
	bSnapToOriginalPositionAfterRelease = true;
}

void UGSItemDrag::SnapToOriginalPosition()
{
	Super::SnapToOriginalPosition();	
}
