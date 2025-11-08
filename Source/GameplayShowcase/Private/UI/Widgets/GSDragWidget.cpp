// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/GSDragWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


FReply UGSDragWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (MainWidgetSlot.IsValid() && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CacheCanvasPropertiesIfNeeded();

		// Set to 1 to hover other widgets when moving
		MainWidgetSlot->SetZOrder(1);
		bIsDragging = true;

		// Offset between widget position and mouse position
		const FVector2D MousePosition = CanvasGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		const FVector2D WidgetPosition = MainWidgetSlot->GetPosition();
		DragOffset = MousePosition - WidgetPosition;
		
		if (const TSharedPtr<SWidget> CachedWidget = GetCachedWidget())
		{
			return FReply::Handled().UseHighPrecisionMouseMovement(CachedWidget.ToSharedRef());
		}
	}
	return FReply::Unhandled();
}

FReply UGSDragWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDragging && MainWidgetSlot.IsValid())
	{
		const FVector2D MousePosition = CanvasGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		FVector2D NewPosition = MousePosition - DragOffset;
		NewPosition.X = FMath::Clamp(NewPosition.X, 0.f, CanvasSize.X - DraggedWidgetSize.X);
		NewPosition.Y = FMath::Clamp(NewPosition.Y, 0.f, CanvasSize.Y - DraggedWidgetSize.Y);	
		MainWidgetSlot->SetPosition(NewPosition);
		
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UGSDragWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (MainWidgetSlot.IsValid() && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		MainWidgetSlot->SetZOrder(0);
		
		bIsDragging = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

void UGSDragWidget::SetMainWidgetRef(UUserWidget* Widget)
{
	DraggedWidget = Widget;
	if (DraggedWidget.IsValid())
	{
		MainWidgetSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DraggedWidget.Get());
		DraggedWidget.Get()->OnVisibilityChanged.AddUniqueDynamic(this, &UGSDragWidget::ReleaseMouseCaptureIfVisibilityChanged);
	}
}

void UGSDragWidget::SetCanvasPanelRef(UCanvasPanel* CanvasPanel)
{
	CanvasPanelRef = CanvasPanel;
}

void UGSDragWidget::CacheCanvasPropertiesIfNeeded()
{
	if (bCachedCanvas)
	{
		return;
	}
	
	if (CanvasPanelRef.IsValid() && MainWidgetSlot.IsValid())
	{
		bCachedCanvas = true;
        CanvasSize = CanvasPanelRef->GetCachedGeometry().GetLocalSize();
        CanvasGeometry = CanvasPanelRef->GetCachedGeometry();
        DraggedWidgetSize = MainWidgetSlot.Get()->GetSize();		
	}		
}

void UGSDragWidget::ReleaseMouseCaptureIfVisibilityChanged(ESlateVisibility NewVisibility)
{
	if (!bIsDragging || !MainWidgetSlot.IsValid())
	{
		return;
	}
		
	if (NewVisibility== ESlateVisibility::Collapsed || NewVisibility == ESlateVisibility::Hidden)
	{
		MainWidgetSlot->SetZOrder(0);
		bIsDragging = false;
			
		FEventReply Reply;
		UWidgetBlueprintLibrary::ReleaseMouseCapture(Reply);
	}
}
