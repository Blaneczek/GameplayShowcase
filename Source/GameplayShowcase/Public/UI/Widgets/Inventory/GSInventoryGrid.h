// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSInventoryGrid.generated.h"

class UCanvasPanel;
struct FItemDefinition;
class UGSGridItem;
struct FGridPosition;
class UUniformGridSlot;
class UUniformGridPanel;
class UGSGridSlot;


/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSInventoryGrid : public UGSWidgetBase
{
	GENERATED_BODY()
	
public:
	void AddItem(const FItemDefinition& ItemDef, const TArray<FGridPosition>& Positions);
	bool FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo);

	void RelocateGridItem(UGSGridItem* GridItem);
	
protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> GridPanel;
	
private:
	UFUNCTION(BlueprintCallable)
	void ConstructGrid();

	void SetSlotsOccupancy(const TArray<FGridPosition>& Positions, bool bOccupied);
	bool CheckAllPositions(int32 RowIndex, int32 ColumnIndex, const FItemSize& ItemSize, TArray<FGridPosition>& Positions);
	UGSGridSlot* GetGridSlotAtPosition(int32 RowIndex, int32 ColumnIndex);

	void CheckProxyPositions(const FGridPosition& Position, const FItemSize& ProxySize);
	void ClearProxyPositions();
	FGridPosition GetFirstGridPosition(const TArray<FGridPosition>& Positions);
	
	UPROPERTY()
	TArray<UGSGridSlot*> GridSlots;
	UPROPERTY()
	TArray<UGSGridItem*> GridItems;

	TArray<FGridPosition> ProxyPositions;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GridSlotClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSGridItem> GridItemClass;
	
	UPROPERTY(EditAnywhere)
	int32 RowsNum = 0;
	UPROPERTY(EditAnywhere)
	int32 ColumnsNum = 0;
	UPROPERTY(EditAnywhere)
	float SlotSize = 0.f;
};
