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
	virtual void SetMovementSpeed(float NewSpeed = 500.f, bool bSetToDefault = false) {};

	UFUNCTION(BlueprintCallable, Category="GAS")
	virtual bool IsAttacking() { return false; }

	UFUNCTION(BlueprintCallable, Category="GAS")
	virtual bool IsWeaponEquipped() { return false; };
};
