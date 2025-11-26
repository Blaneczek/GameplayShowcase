// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemInstance.h"
#include "Systems/Inventory/Items/Data/GSItemDataSubsystem.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"


void FItemInstance::Initialize(FItemDefinition&& NewItemDefinition, int32 InitialStackCount)
{
	ItemDefinition = MoveTemp(NewItemDefinition);
	InstanceID = FGuid::NewGuid();
	StackCount = InitialStackCount;
}

void FItemInstance::Initialize(const FItemDefinition& NewItemDefinition, int32 InitialStackCount)
{
	ItemDefinition = NewItemDefinition;
	InstanceID = FGuid::NewGuid();
	StackCount = InitialStackCount;
}

void FItemInstance::LoadItemData()
{
	if (!ItemDefinition.IsValidDefinition())
	{
		UE_LOG(LogTemp, Warning, TEXT("FItemInstance::LoadItemData | Invalid ItemDefinition (no name tag)"));
		return;
	}

	TArray<FSoftObjectPath> AssetPaths;
	AssetPaths.Reserve(ItemDefinition.Fragments.Num());
	for (auto& Fragment : ItemDefinition.Fragments)
    {
		if (FItemFragment* FragmentPtr = Fragment.GetMutablePtr<FItemFragment>())
		{
			FragmentPtr->AddSoftObjectPath(AssetPaths);
		}
	}

	if (AssetPaths.Num() == 0)
	{
		return;
	}
	
	const UAssetManager& AssetManager = UAssetManager::Get();
	FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
	LoadHandle = StreamableManager.RequestAsyncLoad(
		AssetPaths,
		FStreamableDelegate::CreateLambda([this]()
		{
			OnAssetsLoaded();
		}),
		FStreamableManager::AsyncLoadHighPriority,
		false
	);		
}

void FItemInstance::OnAssetsLoaded()
{
	OnItemLoaded.ExecuteIfBound(this);
}
