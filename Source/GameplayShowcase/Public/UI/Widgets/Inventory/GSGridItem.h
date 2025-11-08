// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSGridItem.generated.h"

class UImage;
struct FItemDefinition;

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridItem : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	void ConstructItem(const FItemDefinition& Def);

	FORCEINLINE const FItemSize& GetGridSize() const { return ItemSize; }
	
private:
	void SetItemSize(const FItemDefinition& Def);
	void SetItemImage(const FItemDefinition& Def);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UImage> AltImage;

	UPROPERTY(EditDefaultsOnly)
	FItemSize ItemSize;
	
};
