// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/GSDragWidget.h"
#include "GSItemDrag.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSItemDrag : public UGSDragWidget
{
	GENERATED_BODY()
	
	UGSItemDrag();

protected:
	virtual void SnapToOriginalPosition() override;
};
