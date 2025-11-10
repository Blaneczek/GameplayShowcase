// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/GSDragWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Controllers/GSOverlayWidgetController.h"


FReply UGSDragWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (MainWidgetSlot && OriginalParentPanelRef.IsValid() && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CachePropertiesIfNeeded();

		// Calculate local to main canvas offset 
		const FGeometry& LocalCanvasGeo = OriginalParentPanelRef.Get()->GetCachedGeometry();	
		LocalToMainOffset = CanvasGeometry.AbsoluteToLocal(LocalCanvasGeo.LocalToAbsolute(FVector2D(0.f, 0.f)));	
		FixedCanvasSize = CanvasSize - LocalToMainOffset;
		
		// Hover other widgets when moving
		MainWidgetSlot->SetZOrder(10);
		bIsDragging = true;

		// Offset between widget position and mouse position
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		const FVector2D WidgetPosition = MainWidgetSlot->GetPosition();
		DragOffset = MousePosition - WidgetPosition;
		
		return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	return FReply::Unhandled();
}

FReply UGSDragWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDragging && MainWidgetSlot)
	{
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		FVector2D NewPosition = MousePosition - DragOffset;

		NewPosition.X = FMath::Clamp(NewPosition.X, -LocalToMainOffset.X, FixedCanvasSize.X - DraggedWidgetSize.X);
		NewPosition.Y = FMath::Clamp(NewPosition.Y, -LocalToMainOffset.Y, FixedCanvasSize.Y - DraggedWidgetSize.Y);
		
		MainWidgetSlot->SetPosition(NewPosition);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UGSDragWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (MainWidgetSlot && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		MainWidgetSlot->SetZOrder(0);	
		bIsDragging = false;
		if (bSnapToOriginalPositionAfterRelease)
		{
			SnapToOriginalPosition();
		}		
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

void UGSDragWidget::SetMainWidgetRef(UUserWidget* inWidget)
{
	DraggedWidget = inWidget;
	if (UUserWidget* Widget = DraggedWidget.Get())
	{
		OriginalParentPanelRef = Widget->GetParent();
		MainWidgetSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DraggedWidget.Get());
		
		// Wait one frame to ensure position is valid
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			if (MainWidgetSlot)
			{
				OriginalPosition = MainWidgetSlot->GetPosition();
			}
		});
		
		Widget->OnVisibilityChanged.AddUniqueDynamic(this, &UGSDragWidget::ReleaseMouseCaptureIfVisibilityChanged);
	}
}

void UGSDragWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UGSOverlayWidgetController* Controller = UGSBlueprintFunctionLibrary::GetOverlayWidgetController(this))
	{
		MainCanvasRef = Controller->CanvasRef;
	}
}

void UGSDragWidget::CachePropertiesIfNeeded()
{
	if (bCanvasCached)
	{
		return;
	}
	
	if (MainCanvasRef.IsValid() && MainWidgetSlot)
	{
		bCanvasCached = true;  
        CanvasGeometry = MainCanvasRef->GetCachedGeometry();
        DraggedWidgetSize = MainWidgetSlot.Get()->GetSize();
		CanvasSize = MainCanvasRef->GetCachedGeometry().GetLocalSize();
	}		
}

void UGSDragWidget::SnapToOriginalPosition()
{
	if (MainWidgetSlot)
	{
        MainWidgetSlot->SetPosition(OriginalPosition);
	}	
}

void UGSDragWidget::ReleaseMouseCaptureIfVisibilityChanged(ESlateVisibility NewVisibility)
{
	if (!bIsDragging || !MainWidgetSlot)
	{
		return;
	}
		
	if (NewVisibility == ESlateVisibility::Collapsed || NewVisibility == ESlateVisibility::Hidden)
	{
		MainWidgetSlot->SetZOrder(0);
		bIsDragging = false;
			
		FEventReply Reply;
		UWidgetBlueprintLibrary::ReleaseMouseCapture(Reply);

		if (bSnapToOriginalPositionAfterRelease)
		{
			SnapToOriginalPosition();
		}
	}
}
