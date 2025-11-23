// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSItemTooltip.generated.h"

struct FItemDefinition;
class UGSInventoryMenuWidgetController;
class UTextBlock;
class UVerticalBoxSlot;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSItemTooltip : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	void InitTooltip(const FItemDefinition& Def, UGSInventoryMenuWidgetController* Controller);	
	void AddWidgetToTooltip(UWidget* Widget);

private:
	void SetLevelInfo(TPair<UTextBlock*, int32> LevelInfo);
	void SetLevelColor(int32 PlayerLevel);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> TooltipBox;

	UPROPERTY()
	TObjectPtr<UTextBlock> LevelTextBlock;
	int32 ItemLevel;

	FLinearColor PermittedItemLevelColor = FLinearColor::Green;
	FLinearColor ForbiddenItemLevelColor = FLinearColor::Red;	
};
