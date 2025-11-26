// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GSItemDefinition.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GSItemDataSubsystem.generated.h"

struct FStreamableHandle;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDataLoadedSignature, UGSItemDataSubsystem* /*temDataSubsystem*/);

/**
 * Subsystem that manages loading and caching of item definitions.
 * Loads all GameItems assets asynchronously and provides access to definitions.
 */
UCLASS(BlueprintType)
class GAMEPLAYSHOWCASE_API UGSItemDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	/** 
	 * Finds item definition by name tag.
	 * @return	pointer to item definition, or nullptr if not found
	 */
	const FItemDefinition* FindItemDefinition(const FGameplayTag& ItemName) const;

	/** 
	 * Checks if item data has finished loading.
	 * @return	true if all assets are loaded and ready for use
	 */
	FORCEINLINE bool IsDataLoaded() const { return bDataLoaded; }

	/** Gets all loaded item definitions. */
	FORCEINLINE const TMap<FGameplayTag, FItemDefinition>& GetAllItemDefinitions() const { return ItemDefinitions; }

	/** Gets number of loaded items. */
	FORCEINLINE int32 GetItemCount() const { return ItemDefinitions.Num(); }
	
	FOnItemDataLoadedSignature OnItemDataLoaded;
	
private:
	/** 
	 * Starts loading all GameItems assets asynchronously.
	 * Fires OnItemDataLoaded delegate when complete.
	 */
    void LoadItemsData();
	void OnAssetsLoaded();
	
	/** Populates item definitions */
	void AddLoadedAssets(const TArray<FPrimaryAssetId>& LoadedAssetIds);
	void FinalizeLoading();
	
	/** Map of all loaded item definitions by name tag. */
	UPROPERTY()
	TMap<FGameplayTag, FItemDefinition> ItemDefinitions;

	TSharedPtr<FStreamableHandle> LoadHandle;
	
	bool bDataLoaded = false;
	
	int32 TotalAssets = 0;
};
