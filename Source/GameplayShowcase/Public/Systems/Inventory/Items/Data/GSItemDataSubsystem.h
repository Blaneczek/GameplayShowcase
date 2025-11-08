// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GSItemDefinition.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GSItemDataSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDataLoadedSignature, UGSItemDataSubsystem*, ItemDataSubsystem);

/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPLAYSHOWCASE_API UGSItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const FItemDefinition* GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	UPROPERTY(BlueprintAssignable)
	FOnItemDataLoadedSignature OnItemDataLoadedDelegate;
	
private:	
    void LoadItemsData();

	UPROPERTY()
	TMap<FGameplayTag, FItemDefinition> ItemDefinitions;

	std::atomic<int32> NumAssets = 0;
};
