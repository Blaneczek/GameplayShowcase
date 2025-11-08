// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Data/GSItemDataSubsystem.h"
#include "Engine/AssetManager.h"
#include "Systems/Inventory/Items/Data/GSItemsInfo.h"

void UGSItemDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadItemsData();
}

const FItemDefinition* UGSItemDataSubsystem::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	if (const FItemDefinition* Item = ItemDefinitions.Find(ItemTag))
	{
		return Item;
	}
	return nullptr;
}

void UGSItemDataSubsystem::LoadItemsData()
{
	UAssetManager& Manager = UAssetManager::Get();
	TArray<FPrimaryAssetId> AssetIds;
	Manager.GetPrimaryAssetIdList(FPrimaryAssetType("GameItems"), AssetIds);

	NumAssets = AssetIds.Num();
	
	for (const FPrimaryAssetId& Id : AssetIds)
	{
		Manager.LoadPrimaryAsset(Id, {}, FStreamableDelegate::CreateLambda([this, Id]()
		{
			UObject* LoadedObj = UAssetManager::Get().GetPrimaryAssetObject(Id);
			if (UGSItemsInfo* DataAsset = Cast<UGSItemsInfo>(LoadedObj))
			{
				for (const FItemDefinition& Item : DataAsset->Items)
				{
					ItemDefinitions.Add(Item.Name, Item);
				}
			}
			
			if (--NumAssets == 0)
			{
				OnItemDataLoadedDelegate.Broadcast(this);
			}		
		}));
	}
}
