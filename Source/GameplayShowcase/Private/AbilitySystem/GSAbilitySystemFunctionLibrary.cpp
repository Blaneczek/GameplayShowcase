// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "AbilitySystem/GSAbilitySystemFunctionLibrary.h"

UGSOverlayWidgetController* UGSAbilitySystemFunctionLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	return GetWidgetControllerFromHUD<UGSOverlayWidgetController>(WorldContextObject);
}

UGSCharacterMenuWidgetController* UGSAbilitySystemFunctionLibrary::GetCharacterMenuWidgetController(
	const UObject* WorldContextObject)
{	
	return GetWidgetControllerFromHUD<UGSCharacterMenuWidgetController>(WorldContextObject);
}




