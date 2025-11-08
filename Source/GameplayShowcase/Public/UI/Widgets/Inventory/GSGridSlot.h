// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GSGridSlot.generated.h"

class UBorder;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	// Debug
	UFUNCTION(BlueprintCallable)
	void OccupieSlot(bool bIsOccupied);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> GridSlot;
};
