// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GSInventoryHelper.generated.h"

class UGSItemComponent;

// This class does not need to be modified.
UINTERFACE()
class UGSInventoryHelper : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEPLAYSHOWCASE_API IGSInventoryHelper
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AddItemOnFloor(UGSItemComponent* ItemComponent) = 0;
	virtual void RemoveItemOnFloor(UGSItemComponent* ItemComponent) = 0;
};
