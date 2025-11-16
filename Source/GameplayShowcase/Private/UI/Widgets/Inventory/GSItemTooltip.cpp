// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSItemTooltip.h"

#include "Components/VerticalBox.h"


void UGSItemTooltip::AddWidgetToTooltip(UWidget* Widget)
{
	TooltipBox->AddChildToVerticalBox(Widget);
}
