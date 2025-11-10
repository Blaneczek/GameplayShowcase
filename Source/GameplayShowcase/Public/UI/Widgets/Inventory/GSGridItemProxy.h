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
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridItemProxy : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitProxy(UTexture2D* ItemIcon, UGSGridItem* GridItem, UCanvasPanelSlot* inSlot, TWeakObjectPtr<UCanvasPanel> Canvas, const FVector2D& inWidgetSize, const FItemSize& inProxySize);
	void StartDragging();
	bool RemoveProxy();
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemProxyIcon;
	
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasSlot;

	TWeakObjectPtr<UCanvasPanel> CanvasPanel;
	TWeakObjectPtr<UGSGridItem> GridItemRef;

	bool bIsDragging = false;
	FVector2D CanvasSize;
	FVector2D WidgetSize;
	FItemSize ProxySize;
};



