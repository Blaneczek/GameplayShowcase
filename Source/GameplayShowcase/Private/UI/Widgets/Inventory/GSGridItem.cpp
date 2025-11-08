// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridItem.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"

void UGSGridItem::ConstructItem(const FItemDefinition& Def)
{
	SetItemSize(Def);
	SetItemImage(Def);
}

void UGSGridItem::SetItemSize(const FItemDefinition& Def)
{
	if (const FGridFragment* GridFrag = Def.GetFragmentByType<FGridFragment>())
	{
		ItemSize = GridFrag->GetGridSize();
	}
	// Item occupies 1 slot
	else
	{
		ItemSize = FItemSize(1,1);
	}
}

void UGSGridItem::SetItemImage(const FItemDefinition& Def)
{
	if (const FImageFragment* ImageFrag = Def.GetFragmentByType<FImageFragment>())
	{
		ItemImage = ImageFrag->GetImage();
	}
	else
	{
		ItemImage = AltImage;
	}
}
