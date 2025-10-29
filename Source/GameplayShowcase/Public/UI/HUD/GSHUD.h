// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GSHUD.generated.h"

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

	void OpenOrCloseCharacterMenu();
	
private:
	template<typename T>
	requires std::is_base_of_v<UGSWidgetControllerBase, T>
	T* CreateOrGetWidgetController(TObjectPtr<T>& Controller, const TSubclassOf<T>& ControllerClass, const FWidgetControllerParams& WCParams)
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
	
protected:
	UPROPERTY()
	TObjectPtr<UGSWidgetBase> OverlayWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSWidgetBase> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGSOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UGSCharacterMenuWidgetController> CharacterMenuWidgetController;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGSCharacterMenuWidgetController> CharacterMenuWidgetControllerClass;
};
