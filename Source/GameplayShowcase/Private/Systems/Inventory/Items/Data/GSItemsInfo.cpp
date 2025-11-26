// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Data/GSItemsInfo.h"

FItemDefinition UGSItemsInfo::FindItemDefinitionByTag(const FGameplayTag& Tag) const
{
	const auto FoundItem = Items.FindByPredicate(
		[Tag](const FItemDefinition& Item)
		{
			return Item.Name.MatchesTagExact(Tag);
		});

	if (FoundItem)
	{
		return *FoundItem;
	}

	UE_LOG(LogTemp, Warning, TEXT("UGSItemsInfo::FindItemDefinitionByTag | Item with tag [%s] was not found."), *Tag.ToString());
	return FItemDefinition();
}

