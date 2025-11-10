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
	void SetMainWidgetRef(UUserWidget* inWidget);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSnapToOriginalPositionAfterRelease = false;
	
	virtual void SnapToOriginalPosition();
	
private:
	void CachePropertiesIfNeeded();
	
	UFUNCTION()
	void ReleaseMouseCaptureIfVisibilityChanged(ESlateVisibility NewVisibility);
	
	bool bCanvasCached = false;
	bool bIsDragging = false;
	
    TWeakObjectPtr<UCanvasPanel> MainCanvasRef;	
    TWeakObjectPtr<UPanelWidget> OriginalParentPanelRef;
	TWeakObjectPtr<UUserWidget> DraggedWidget;
	
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> MainWidgetSlot;
	
	FGeometry CanvasGeometry;
	FVector2D CanvasSize;
	FVector2D FixedCanvasSize;
	FVector2D DraggedWidgetSize;
	FVector2D DragOffset;
	FVector2D LocalToMainOffset;

	FVector2D OriginalPosition;
};
