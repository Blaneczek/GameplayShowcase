// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"

UGSOverlayWidgetController* UGSBlueprintFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	return GetWidgetControllerFromHUD<UGSOverlayWidgetController>(WorldContextObject);
}

UGSCharacterMenuWidgetController* UGSBlueprintFunctionLibrary::GetCharacterMenuWidgetController(const UObject* WorldContextObject)
{	
	return GetWidgetControllerFromHUD<UGSCharacterMenuWidgetController>(WorldContextObject);
}

UGSInventoryMenuWidgetController* UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(const UObject* WorldContextObject)
{	
	return GetWidgetControllerFromHUD<UGSInventoryMenuWidgetController>(WorldContextObject);
}




