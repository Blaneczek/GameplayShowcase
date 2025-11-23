// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemInstance.h"

#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"


void FItemInstance::Initialize(FItemDefinition&& NewItemDefinition, int32 InitialStackCount)
{
	ItemDefinition = MoveTemp(NewItemDefinition);
	InstanceID = FGuid::NewGuid();
	StackCount = InitialStackCount;
	LoadItemData();
}

void FItemInstance::Initialize(const FItemDefinition& NewItemDefinition, int32 InitialStackCount)
{
	ItemDefinition = NewItemDefinition;
	InstanceID = FGuid::NewGuid();
	StackCount = InitialStackCount;
	LoadItemData();
}

void FItemInstance::LoadItemData()
{
	if (!ItemDefinition.Name.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FItemInstance::LoadItemData | Invalid ItemDefinition (no name tag)"));
		return;
	}
	
	for (auto& Fragment : ItemDefinition.Fragments)
    {
		if (FItemFragment* FragmentPtr = Fragment.GetMutablePtr<FItemFragment>())
		{
			FragmentPtr->LoadData();
		}
	}	
}
