// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetBase.h"
#include "GSCharacterMenuWidget.generated.h"


/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSCharacterMenuWidget : public UGSWidgetBase
{
	GENERATED_BODY()
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
