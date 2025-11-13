// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "GSGridItemProxy.generated.h"

class UGSGridItem;
class UCanvasPanel;
class UCanvasPanelSlot;
class UGSDragWidget;

/**
 * A temporary widget that represents a GridItem while it's being dragged
 * across the inventory UI.
 * - Displays the item icon while dragging.
 * - Follows the mouse cursor each frame.
 * - Notifies the controller when removed (drag cancelled or completed).
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridItemProxy : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initializes the proxy widget after creation.
	 *
	 * @param ItemIcon     texture used as the visual icon of the proxy.
	 * @param GridItem     reference to the grid item this proxy represents.
	 * @param InSlot       canvas slot of this widget.
	 * @param InCanvas     weak pointer to the canvas panel owning this widget.
	 * @param InWidgetSize real size of this widget in canvas panel.
	 * @param InProxySize  size of this proxy in grid units (rows x columns).
	 */
	void InitProxy(UTexture2D* ItemIcon, UGSGridItem* GridItem, UCanvasPanelSlot* InSlot,
					TWeakObjectPtr<UCanvasPanel> InCanvas, const FVector2D& InWidgetSize,
					const FItemSize& InProxySize);

	/** Removes the proxy and notifies controller.
	 *
	 * @return true if the proxy was removed.
	 */
	bool RemoveProxy();
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	virtual void NativeDestruct() override;

private:
	/** Enables drag behavior and positions the widget under the cursor. */
	void InitDragging();

	/** Size of Overlay canvas panel. */
	FVector2D CanvasSize = FVector2D::ZeroVector;

	/** Real size of this widget. */
	FVector2D WidgetSize = FVector2D::ZeroVector;
	
	/** Size of the proxy in grid units (rows x columns). */
    FItemSize ProxySize;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemProxyIcon;

	/** CanvasPanelSlot of this widget. */
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasSlot;

	/** Overlay canvas panel reference. */
	TWeakObjectPtr<UCanvasPanel> CanvasPanel;
	
	/** GridItem reference this proxy represents. */
	TWeakObjectPtr<UGSGridItem> GridItemRef;

	bool bIsDragging = false;
};



