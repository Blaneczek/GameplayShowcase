// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/UI/HUD/GSHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/Controllers/GSAttributeMenuWidgetController.h"
#include "UI/Controllers/GSOverlayWidgetController.h"
#include "UI/Widgets/GSWidgetBase.h"

AGSHUD::AGSHUD()
{
}

UGSOverlayWidgetController* AGSHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UGSOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}	
	return OverlayWidgetController;
}

UGSAttributeMenuWidgetController* AGSHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UGSAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}	
	return AttributeMenuWidgetController;
}

void AGSHUD::InitializeOverlayWidget(APlayerController* PC, ACharacter* PlayerChar,
	UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	if (!OverlayWidgetClass || !OverlayWidgetControllerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GSHUD::InitializeOverlayWidget | OverlayWidgetClass or OverlayWidgetControllerClass is not valid."));
		return;
	}
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UGSWidgetBase>(Widget);

	const FWidgetControllerParams WidgetControllerParams{PC, PlayerChar, ASC, AS};
	UGSOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	OverlayWidget->AddToViewport();
}