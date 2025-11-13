// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemInstance.h"

#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"


void FItemInstance::MoveItemDefinition(FItemDefinition&& NewItemDefinition)
{
	ItemDefinition = TInstancedStruct<FItemDefinition>::Make(MoveTemp(NewItemDefinition));
	
	LoadItemData();
}

void FItemInstance::CopyItemDefinition(const FItemDefinition& NewItemDefinition)
{
	ItemDefinition = TInstancedStruct<FItemDefinition>::Make(NewItemDefinition);
	
	LoadItemData();
}

void FItemInstance::LoadItemData()
{
	FItemDefinition& Definition = GetItemDefinitionMutable();
	for (auto& Fragment : Definition.Fragments)
	{
		Fragment.GetMutable<FItemFragment>().LoadData();
	}
}
