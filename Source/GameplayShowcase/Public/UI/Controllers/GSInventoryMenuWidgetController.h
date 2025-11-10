// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "GSInventoryMenuWidgetController.generated.h"

class UGSGridItemProxy;
class UGSGridItem;
struct FItemSize;

struct FGridInfo
{
	FGridInfo() = default;
	FGridInfo(int32 inGridIndex, const TArray<FGridPosition>& inPositions)
		: GridIndex(inGridIndex), Positions(inPositions)
	{}
	
	int32 GridIndex = 0;
	TArray<FGridPosition> Positions;
};

DECLARE_DELEGATE_TwoParams(FCreateNewItemSignature, const FItemDefinition& ItemDef, const FGridInfo& GridInfo);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FFindNewSpaceDelegate, const FItemSize& /*ItemSize*/, FGridInfo& /*OutGridInfo*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemProxyStatusChangedSignature, bool bProxyExists, const FItemSize& ProxySize);
DECLARE_DELEGATE_TwoParams(FRelocateGridItemSignature, int32 GridIndex, UGSGridItem* GridItem);


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYSHOWCASE_API UGSInventoryMenuWidgetController : public UGSWidgetControllerBase
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
	bool FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo);
	void CallOnGridItemProxyStatusChanged(bool bProxyExists, const FItemSize& ProxySize);
	void TryRelocateItemGrid(int32 GridIndex);
	void SetProxyGridItem(UGSGridItem* GridItem);
	
	FCreateNewItemSignature CreateNewItemDelegate;
	FFindNewSpaceDelegate FindNewSpaceDelegate;
	FItemProxyStatusChangedSignature OnItemProxyStatusChanged;
	FRelocateGridItemSignature RelocateGridItemDelegate;
	
private:
	TWeakObjectPtr<UGSInventoryComponent> InventoryComponent;
	TWeakObjectPtr<UGSGridItem> GridItemRef;
};
