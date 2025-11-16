// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSItemTooltip.generated.h"

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
	void AddWidgetToTooltip(UWidget* Widget);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> TooltipBox;
};
