// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/UI/Widgets/GSWidgetBase.h"

void UGSWidgetBase::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
