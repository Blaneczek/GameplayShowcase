// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "UI/Controllers/GSWidgetControllerBase.h"
#include "UI/HUD/GSHUD.h"
#include "GSBlueprintFunctionLibrary.generated.h"


class UGSLevelUpInfo;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "GSAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UGSOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GSAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UGSCharacterMenuWidgetController* GetCharacterMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "GSAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UGSInventoryMenuWidgetController* GetInventoryMenuWidgetController(const UObject* WorldContextObject);
	
private:
	// TODO: Optimize this
	template<typename T>
	static T* GetWidgetControllerFromHUD(const UObject* WorldContextObject)
	{
		if (!WorldContextObject)
		{
			return nullptr;
		}

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
		{
			if (AGSHUD* GSHUD = PC->GetHUD<AGSHUD>())
			{
				if (AGSPlayerCharacterBase* Char = Cast<AGSPlayerCharacterBase>(PC->GetCharacter()))
				{
					UAbilitySystemComponent* ASC = Char->GetAbilitySystemComponent();
					UAttributeSet* AS = Char->GetAttributeSet();

					FWidgetControllerParams WCParams(PC, Char, ASC, AS);

					if constexpr (std::is_same_v<T, UGSOverlayWidgetController>)
					{
						return GSHUD->GetOverlayWidgetController(WCParams);
					}
					else if constexpr (std::is_same_v<T, UGSCharacterMenuWidgetController>)
					{
						return GSHUD->GetCharacterMenuWidgetController(WCParams);
					}
					else if constexpr (std::is_same_v<T, UGSInventoryMenuWidgetController>)
					{
						return GSHUD->GetInventoryMenuWidgetController(WCParams);
					}
					else
					{
						static_assert(false, "Unsupported Widget Controller type");
					}
				}
			}
		}
		return nullptr;
	}
};
