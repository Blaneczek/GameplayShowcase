// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/GSHUDHelper.h"
#include "UI/Widgets/GSCharacterMenuWidget.h"
#include "UI/Widgets/GSInventoryMenuWidget.h"
#include "GSHUD.generated.h"

class UGSInventoryMenuWidgetController;
class UGSWidgetControllerBase;
class UAttributeSet;
struct FWidgetControllerParams;
class UGSWidgetBase;
class UGSCharacterMenuWidgetController;
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
	UGSCharacterMenuWidgetController* GetCharacterMenuWidgetController(const FWidgetControllerParams& WCParams);
	UGSInventoryMenuWidgetController* GetInventoryMenuWidgetController(const FWidgetControllerParams& WCParams);

	template <typename T>
	void OpenOrCloseMenuByType();
	
protected:
	UPROPERTY()
	TObjectPtr<UGSWidgetBase> OverlayWidget;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGSWidgetBase> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGSOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGSOverlayWidgetController> OverlayControllerClass;

	UPROPERTY()
	TObjectPtr<UGSCharacterMenuWidgetController> CharacterMenuController;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGSCharacterMenuWidgetController> CharacterMenuControllerClass;

	UPROPERTY()
	TObjectPtr<UGSInventoryMenuWidgetController> InventoryMenuController;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGSInventoryMenuWidgetController> InventoryMenuControllerClass;

private:
	template<typename T> requires std::is_base_of_v<UGSWidgetControllerBase, T>
	T* CreateOrGetWidgetControllerByType(TObjectPtr<T>& Controller, const TSubclassOf<T>& ControllerClass, const FWidgetControllerParams& WCParams);
	
};

template<typename T> requires std::is_base_of_v<UGSWidgetControllerBase, T>
T* AGSHUD::CreateOrGetWidgetControllerByType(TObjectPtr<T>& Controller, const TSubclassOf<T>& ControllerClass, const FWidgetControllerParams& WCParams)
{
	if (Controller == nullptr)
	{
		checkf(ControllerClass, TEXT("AGSHUD::GetWidgetController | ControllerClass is not valid"));
		Controller = NewObject<T>(this, ControllerClass);
		Controller->SetWidgetControllerParams(WCParams);
		Controller->BindCallbacksToDependencies();
	}
	return Controller;
}

template <typename T>
void AGSHUD::OpenOrCloseMenuByType()
{
	if (!OverlayWidget)
	{
		return;
	}
	
	if constexpr (std::is_same_v<T, UGSCharacterMenuWidget>)
	{
		IGSHUDHelper::Execute_OpenOrCloseCharacterMenu(OverlayWidget);
	}
	else if constexpr (std::is_same_v<T, UGSInventoryMenuWidget>)
	{
		IGSHUDHelper::Execute_OpenOrCloseInventoryMenu(OverlayWidget);
	}
	else
	{
		static_assert(false, "Unsupported Widget type.");
	}
}
