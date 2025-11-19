// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSItemTooltip.h"

#include "GSBlueprintFunctionLibrary.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Systems/Inventory/Items/Data/GSItemDefinition.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"


void UGSItemTooltip::InitTooltip(const FItemDefinition& Def, UGSInventoryMenuWidgetController* Controller)
{
	SetVisibility(ESlateVisibility::Collapsed);
	
	TArray<const FWidgetFragment*> Fragments = Def.GetAllFragmentsByType<FWidgetFragment>();

	Def.AdaptItemNameToWidget(this);
	SetLevelInfo(Def.AdaptItemLevelToWidget(this));
	for (const auto& Fragment : Fragments)
	{
		Fragment->AdaptToWidget(this);
	}
	Def.AdaptItemTypeToWidget(this);

	if (Controller)
	{
		Controller->OnPlayerLevelChanged.AddUObject(this, &UGSItemTooltip::SetLevelColor);
	}	
}

void UGSItemTooltip::AddWidgetToTooltip(UWidget* Widget)
{
	TooltipBox->AddChildToVerticalBox(Widget);
}

void UGSItemTooltip::SetLevelInfo(TPair<UTextBlock*, int32> LevelInfo)
{
	LevelTextBlock = LevelInfo.Key;
	ItemLevel = LevelInfo.Value;

	if (LevelTextBlock && ItemLevel <= 0)
	{
		LevelTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	SetLevelColor(UGSBlueprintFunctionLibrary::GetPlayerLevel(this));
}

void UGSItemTooltip::SetLevelColor(int32 PlayerLevel)
{
	if (!LevelTextBlock)
	{
		return;
	}
	
	PlayerLevel >= ItemLevel
	? LevelTextBlock->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f,1.f))
	: LevelTextBlock->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f,1.f));
}

