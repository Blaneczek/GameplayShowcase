// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSAbilityCharacterHelper.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UGSAbilityCharacterHelper : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYSHOWCASE_API IGSAbilityCharacterHelper 
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="GAS")
	virtual void SetMovementSpeed(bool bSprint = true, float NewSpeed = 500.f) {};
};
