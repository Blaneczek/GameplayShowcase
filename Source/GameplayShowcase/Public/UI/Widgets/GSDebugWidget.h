// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GSDebugWidget.generated.h"

class UGSLevelingComponent;
/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSDebugWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized();
	
	UPROPERTY(BlueprintReadOnly, Category="Leveling")
	TObjectPtr<UGSLevelingComponent> LevelingComponent;

};
