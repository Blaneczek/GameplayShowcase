// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetBase.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "GSInventoryMenuWidget.generated.h"

struct FItemDefinition;
struct FGridPosition;
class UGSInventoryMenuWidgetController;
class UGSInventoryGrid;


/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSInventoryMenuWidget : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	bool FindFreeSpaceForItem(const FItemSize& ItemSize, FGridInfo& OutGridInfo);

protected:
	UFUNCTION(BlueprintCallable)
	void RegisterInventoryGrid(int32 Index, UGSInventoryGrid* NewInventoryGrid);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGSInventoryMenuWidgetController> InventoryController;
	
private:
	void AddItem(const FItemDefinition& ItemDef, const FGridInfo& GridInfo);
	void RelocateGridItem(int32 GridIndex, UGSGridItem* GridItem);
	
	UPROPERTY()
	TMap<int32, UGSInventoryGrid*> InventoryGrids;
};
