// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "GSInventoryMenuWidgetController.generated.h"

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

DECLARE_MULTICAST_DELEGATE_TwoParams(FCreateNewItemSignature, const FItemDefinition& ItemDef, const FGridInfo& GridInfo);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FFindNewSpaceDelegate, const FItemSize& /*ItemSize*/, FGridInfo& /*OutGridInfo*/);


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

	FCreateNewItemSignature CreateNewItemDelegate;
	FFindNewSpaceDelegate FindNewSpaceDelegate;
	
	bool FindFreeSpace(const FItemSize& ItemSize, FGridInfo& OutGridInfo);
	
private:
	TWeakObjectPtr<UGSInventoryComponent> InventoryComponent;
};
