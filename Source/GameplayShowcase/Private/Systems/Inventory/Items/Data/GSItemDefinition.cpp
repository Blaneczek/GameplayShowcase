// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Data/GSItemDefinition.h"

#include "GSBlueprintFunctionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Systems/Inventory/Items/GSItemComponent.h"
#include "Systems/Inventory/Items/GSItemTags.h"
#include "UI/Widgets/Inventory/GSItemTooltip.h"

void FItemDefinition::SpawnWorldActor(const UObject* WorldContextObject, const FTransform& SpawnTransform)
{
	const TSubclassOf<AActor> Class = WorldActorClass.LoadSynchronous();
	if (!Class || !WorldContextObject)
	{
		return;
	}

	if (const AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(Class, SpawnTransform))
	{
		UGSItemComponent* ItemComp = UGSItemComponent::FindItemComponent(SpawnedActor);
		checkf(ItemComp, TEXT("FItemDefinition::SpawnWorldActor | Item component not found in spawned actor"));

		//TODO: ItemComp->InitItemDefinition(*this);
	}
}

void FItemDefinition::AdaptItemNameToWidget(UGSItemTooltip* ItemTooltip)  const
{
	if (UTextBlock* NameText = NewObject<UTextBlock>(ItemTooltip))
	{
		FSlateFontInfo FontInfo = NameText->GetFont();
		FontInfo.Size = 8;
		NameText->SetFont(FontInfo);
		NameText->SetJustification(ETextJustify::Center);
		NameText->SetColorAndOpacity(FLinearColor(1.f, 0.7f, 0.f,1.f));
	
		NameText->SetText(UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Name));
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

void FItemDefinition::ClearFragments()
{
	Fragments.Empty();
}
