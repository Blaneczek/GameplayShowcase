// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSGridItem.generated.h"

class UCanvasPanelSlot;
class UGSGridItemProxy;
class UGSDragWidget;
class UImage;
struct FItemDefinition;

DECLARE_DELEGATE_OneParam(FOnProxyStatusChanged, bool bProxyExists);

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridItem : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	void ConstructItem(const FItemDefinition& Def, float inSlotSize, const TArray<FGridPosition>& Positions);

	FORCEINLINE FItemSize GetGridSize() const { return ItemSize; }
	FORCEINLINE UGSGridItemProxy* GetItemProxy() const { return ItemProxy; }
	
	FORCEINLINE const TArray<FGridPosition>& GetGridPositions() const { return GridPositions; }
	FORCEINLINE void SetGridPositions(const TArray<FGridPosition>& Positions)  { GridPositions = Positions; }
	
	FORCEINLINE UCanvasPanelSlot* GetCanvasSlot() const { return CanvasSlotRef; }
	FORCEINLINE void SetCanvasSlot(UCanvasPanelSlot* CanvasSlot) { CanvasSlotRef = CanvasSlot; }
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FOnProxyStatusChanged OnProxyStatusChanged;
	
protected:
	UPROPERTY(EditAnywhere)
    TSubclassOf<UGSGridItemProxy> ItemProxyClass;
	
private:
	void SetItemSize(const FItemDefinition& Def);
	void SetItemImage(const FItemDefinition& Def);

	void CreateItemProxy();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> AltIcon;
	
	UPROPERTY()
	TObjectPtr<UGSGridItemProxy> ItemProxy;
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasSlotRef;
	
	TArray<FGridPosition> GridPositions;
		
	FItemSize ItemSize;
	float SlotSize = 0.f;
};

