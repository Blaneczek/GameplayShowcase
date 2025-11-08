// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemInstance.h"


void FItemInstance::MoveItemDefinition(FItemDefinition&& NewItemDefinition)
{
	ItemDefinition = TInstancedStruct<FItemDefinition>::Make(MoveTemp(NewItemDefinition));
}

void FItemInstance::CopyItemDefinition(const FItemDefinition& NewItemDefinition)
{
	ItemDefinition = TInstancedStruct<FItemDefinition>::Make(NewItemDefinition);
}
