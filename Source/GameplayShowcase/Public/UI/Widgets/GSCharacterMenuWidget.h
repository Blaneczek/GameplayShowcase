// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetBase.h"
#include "GSCharacterMenuWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWidgetClicked, const FPointerEvent&, MouseEvent);

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSCharacterMenuWidget : public UGSWidgetBase
{
	GENERATED_BODY()
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintAssignable)
	FWidgetClicked OnWidgetClicked;
};
