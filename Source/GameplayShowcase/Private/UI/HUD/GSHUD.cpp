// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/UI/HUD/GSHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Controllers/GSCharacterMenuWidgetController.h"
#include "UI/Controllers/GSOverlayWidgetController.h"
#include "UI/Widgets/GSWidgetBase.h"

AGSHUD::AGSHUD()
{
}

UGSOverlayWidgetController* AGSHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	return CreateOrGetWidgetController<UGSOverlayWidgetController>(OverlayWidgetController, OverlayWidgetControllerClass, WCParams);
}

UGSCharacterMenuWidgetController* AGSHUD::GetCharacterMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	return CreateOrGetWidgetController<UGSCharacterMenuWidgetController>(CharacterMenuWidgetController, CharacterMenuWidgetControllerClass, WCParams);
}

void AGSHUD::OpenOrCloseCharacterMenu()
{
	if (OverlayWidget)
	{
		IGSHUDHelper::Execute_OpenOrCloseCharacterMenu(OverlayWidget); 
	}
}

void AGSHUD::InitializeOverlayWidget(APlayerController* PC, ACharacter* PlayerChar,
                                     UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	if (!OverlayWidgetClass || !OverlayWidgetControllerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GSHUD::InitializeOverlayWidget | OverlayWidgetClass or OverlayWidgetControllerClass is not valid."));
		return;
	}
	
	OverlayWidget = CreateWidget<UGSWidgetBase>(GetWorld(), OverlayWidgetClass);

	const FWidgetControllerParams WidgetControllerParams{PC, PlayerChar, ASC, AS};
	UGSOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	OverlayWidget->AddToViewport();
}
