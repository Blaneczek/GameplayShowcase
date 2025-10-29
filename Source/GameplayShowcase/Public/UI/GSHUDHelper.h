// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSHUDHelper.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UGSHUDHelper : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYSHOWCASE_API IGSHUDHelper
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="GSHUD|Helper")
	void OpenOrCloseCharacterMenu();
};
