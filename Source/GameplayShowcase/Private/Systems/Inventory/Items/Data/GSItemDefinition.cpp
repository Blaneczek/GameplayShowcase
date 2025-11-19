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
	if (UTextBlock* NameText = NewObject<UTextBlock>(ItemTooltip))
	{
		if (const FEquipmentFragment* EquipFragment = GetFragmentByType<FEquipmentFragment>())
		{
			const FText Text = FText::Format(FText::FromString(TEXT("{0}+{1}"))
				, UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Name)
				, EquipFragment->UpgradeLevel);	
			NameText->SetText(Text);
		}
		else
		{
			NameText->SetText(UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Name));
		}

		FSlateFontInfo FontInfo = NameText->GetFont();
		FontInfo.Size = 8;
		NameText->SetFont(FontInfo);
		NameText->SetJustification(ETextJustify::Center);
		NameText->SetColorAndOpacity(FLinearColor(1.f, 0.7f, 0.f,1.f));
	
		
		ItemTooltip->AddWidgetToTooltip(NameText);
	}
}

void FItemDefinition::AdaptItemTypeToWidget(UGSItemTooltip* ItemTooltip) const
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = 8;
		TextBlock->SetFont(FontInfo);
		TextBlock->SetJustification(ETextJustify::Center);
		const FText Text = FText::Format(FText::FromString(TEXT("[{0}]")), UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Type));	
		TextBlock->SetText(Text);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
		UVerticalBoxSlot* Slot = UWidgetLayoutLibrary::SlotAsVerticalBoxSlot(TextBlock);
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Slot->SetVerticalAlignment(VAlign_Bottom);
	}
}

TPair<UTextBlock*, int32> FItemDefinition::AdaptItemLevelToWidget(UGSItemTooltip* ItemTooltip) const
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{		
		FSlateFontInfo FontInfo = TextBlock ->GetFont();
		FontInfo.Size = 8;
		TextBlock->SetFont(FontInfo);
		TextBlock->SetJustification(ETextJustify::Center);
		TextBlock->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f,1.f));
		const FText Text = FText::Format(FText::FromString(TEXT("Level: {0}")), Level);	
		TextBlock->SetText(Text);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
		return TPair<UTextBlock*, int32>(TextBlock, Level);
	}
	return TPair<UTextBlock*, int32>(nullptr, Level);
}

void FItemDefinition::ClearFragments()
{
	Fragments.Empty();
}
