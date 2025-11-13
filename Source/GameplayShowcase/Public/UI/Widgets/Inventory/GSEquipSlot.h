// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSGridSlot.h"
#include "GSEquipSlot.generated.h"

/**
 * Represents a single grid cell within the equipment panel.
 * Responsible for tracking its occupancy and handling mouse events.
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSEquipSlot : public UGSGridSlot
{
	GENERATED_BODY()

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override {};
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override {};
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
