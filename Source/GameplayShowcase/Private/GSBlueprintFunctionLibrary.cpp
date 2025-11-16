// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "GSBlueprintFunctionLibrary.h"
#include "Components/CanvasPanel.h"
#include "UI/Controllers/GSOverlayWidgetController.h"

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

UCanvasPanel* UGSBlueprintFunctionLibrary::GetOverlayCanvasPanelRef(const UObject* WorldContextObject)
{
	const UGSOverlayWidgetController* Controller = GetOverlayWidgetController(WorldContextObject);
	return Controller ? Controller->GetCanvasPanelRef() : nullptr;	
}

FText UGSBlueprintFunctionLibrary::GetGameplayTagAsText(const FGameplayTag& Tag)
{
	const FString FullName = Tag.ToString();
	int32 DotIndex;
	FullName.FindLastChar('.', DotIndex);
	const FString ShortName = FullName.Mid(DotIndex + 1);
	
	return FText::FromString(ShortName);	
}




