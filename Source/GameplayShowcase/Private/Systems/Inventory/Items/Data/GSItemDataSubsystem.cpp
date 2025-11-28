// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Data/GSItemDataSubsystem.h"
#include "Engine/AssetManager.h"
#include "Systems/Inventory/Items/Data/GSItemsInfo.h"

void UGSItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Number of items in the game
	ItemDefinitions.Reserve(50);
	LoadItemsData();
}

void UGSItemDataSubsystem::Deinitialize()
{
	if (LoadHandle.IsValid())
	{
		LoadHandle->ReleaseHandle();
		LoadHandle.Reset();
	}
	ItemDefinitions.Empty();
	
	Super::Deinitialize();
}

const FItemDefinition* UGSItemDataSubsystem::FindItemDefinition(const FGameplayTag& ItemName) const
{
	return ItemDefinitions.Find(ItemName);
}

void UGSItemDataSubsystem::LoadItemsData()
{
	if (LoadHandle.IsValid() && LoadHandle->IsLoadingInProgress())
	{
		return;
	}

	ItemDefinitions.Empty();
	bDataLoaded = false;
	TotalAssets = 0;

	const UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("GameItems"), AssetIds);

	TotalAssets = AssetIds.Num();
	if (TotalAssets == 0)
	{
		FinalizeLoading();
		return;
	}

	TArray<FSoftObjectPath> AssetPaths;
	AssetPaths.Reserve(TotalAssets);
	for (const auto& Id : AssetIds)
	{
		AssetPaths.Add(AssetManager.GetPrimaryAssetPath(Id));
	}

	FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
	LoadHandle = StreamableManager.RequestAsyncLoad(
		AssetPaths,
		FStreamableDelegate::CreateUObject(this, &UGSItemDataSubsystem::OnAssetsLoaded),
		FStreamableManager::AsyncLoadHighPriority,
		false
	);
	
	if (!LoadHandle.IsValid())
	{
		FinalizeLoading();
	}
}

void UGSItemDataSubsystem::OnAssetsLoaded()
{
	if (!LoadHandle.IsValid())
	{
		FinalizeLoading();
		return;
	}

	const UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("GameItems"), AssetIds);

	AddLoadedAssets(AssetIds);
	FinalizeLoading();
}

void UGSItemDataSubsystem::AddLoadedAssets(const TArray<FPrimaryAssetId>& LoadedAssetIds)
{
	const UAssetManager& AssetManager = UAssetManager::Get();	
	for (const auto& Id : LoadedAssetIds)
	{
		UObject* LoadedObj = AssetManager.GetPrimaryAssetObject(Id);
		if (!LoadedObj)
		{
			continue;
		}
		UGSItemsInfo* DataAsset = Cast<UGSItemsInfo>(LoadedObj);
		if (!DataAsset || DataAsset->Items.Num() == 0)
		{
			continue;
		}

		for (const auto& Item : DataAsset->Items)
		{
			if (!Item.IsValidDefinition() || ItemDefinitions.Contains(Item.Name))
			{
				continue;
			}
			ItemDefinitions.Add(Item.Name, Item);
		}
	}
}

void UGSItemDataSubsystem::FinalizeLoading()
{
	bDataLoaded = true;
	OnItemDataLoaded.Broadcast(this);
}
