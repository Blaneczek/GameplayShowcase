// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/GSCharacterMenuWidget.h"

FReply UGSCharacterMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnWidgetClicked.Broadcast(InMouseEvent);
	return FReply::Handled();
}
