// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GSDragWidget.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSDragWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable)
	void SetMainWidgetRef(UUserWidget* Widget);
	UFUNCTION(BlueprintCallable)
	void SetCanvasPanelRef(UCanvasPanel* CanvasPanel);

private:
	void CacheCanvasPropertiesIfNeeded();

	UFUNCTION()
	void ReleaseMouseCaptureIfVisibilityChanged(ESlateVisibility NewVisibility);
	
	bool bCachedCanvas = false;
	bool bIsDragging = false;
	
    TWeakObjectPtr<UCanvasPanel> CanvasPanelRef;	
	TWeakObjectPtr<UCanvasPanelSlot> MainWidgetSlot;
	TWeakObjectPtr<UUserWidget> DraggedWidget;
	
	FGeometry CanvasGeometry;
	FVector2D DragOffset;
	FVector2D CanvasSize;
	FVector2D DraggedWidgetSize;
};
