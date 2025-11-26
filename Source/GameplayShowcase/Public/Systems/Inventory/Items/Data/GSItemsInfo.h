// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSItemDefinition.h"
#include "Engine/DataAsset.h"
#include "GSItemsInfo.generated.h"

/**
 * Data asset for game items
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSItemsInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FItemDefinition FindItemDefinitionByTag(const FGameplayTag& Tag) const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("GameItems", GetFName()); }
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FItemDefinition> Items;
};
