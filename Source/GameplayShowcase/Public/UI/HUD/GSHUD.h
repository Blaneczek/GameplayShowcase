// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GSHUD.generated.h"

class UAttributeSet;
struct FWidgetControllerParams;
class UGSWidgetBase;
class UGSAttributeMenuWidgetController;
class UGSOverlayWidgetController;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API AGSHUD : public AHUD
{
	GENERATED_BODY()
public:
	AGSHUD();

	void InitializeOverlayWidget(APlayerController* PC, ACharacter* PlayerChar, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UGSOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UGSAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	
private:
	UPROPERTY()
	TObjectPtr<UGSWidgetBase> OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSWidgetBase> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGSOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UGSAttributeMenuWidgetController> AttributeMenuWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
