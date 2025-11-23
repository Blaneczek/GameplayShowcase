// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Data/GSItemDefinition.h"

#include "GSBlueprintFunctionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Widgets/Inventory/GSItemTooltip.h"


void FItemDefinition::AdaptItemNameToWidget(UGSItemTooltip* ItemTooltip)  const
{
	FText Text;
	const FEquipmentFragment* EquipFragment = FindFragmentByType<FEquipmentFragment>();

	EquipFragment
	? Text = FText::Format(FText::FromString(TEXT("{0}+{1}")), UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Name), EquipFragment->GetUpgradeLevel())
	: Text = UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Name);
	
	AdaptTextBlock(ItemTooltip, Text, ItemNameColor);
}

void FItemDefinition::AdaptItemTypeToWidget(UGSItemTooltip* ItemTooltip) const
{
	const FText Text = FText::Format(FText::FromString(TEXT("[{0}]")), UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Type));
	if (UTextBlock* TextBlock = AdaptTextBlock(ItemTooltip, Text, FLinearColor::White))
	{
		UVerticalBoxSlot* Slot = UWidgetLayoutLibrary::SlotAsVerticalBoxSlot(TextBlock);
        Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        Slot->SetVerticalAlignment(VAlign_Bottom);
	}
}

TPair<UTextBlock*, int32> FItemDefinition::AdaptItemLevelToWidget(UGSItemTooltip* ItemTooltip) const
{
	const FText Text = FText::Format(FText::FromString(TEXT("Level: {0}")), Level);
	return TPair<UTextBlock*, int32>(AdaptTextBlock(ItemTooltip, Text, PermittedItemLevelColor), Level);
}

void FItemDefinition::ClearFragments()
{
	Fragments.Empty();
}

void FItemDefinition::SetTextFont(UTextBlock* TextBlock, const FLinearColor& TextColor) const 
{
	if (!TextBlock)
	{
		return;
	}
	
	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.Size = 8;
	TextBlock->SetFont(FontInfo);
	TextBlock->SetJustification(ETextJustify::Center);
	TextBlock->SetColorAndOpacity(TextColor);
}

UTextBlock* FItemDefinition::AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text, const FLinearColor& TextColor) const
{
	if (!ItemTooltip)
	{
		return nullptr;
	}
	
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock, TextColor);
		TextBlock->SetText(Text);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
		return TextBlock;
	}
	return nullptr;
}
