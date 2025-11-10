// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Widgets/GSWidgetBase.h"
#include "GSGridItem.generated.h"

class UGSGridItemProxy;
class UGSDragWidget;
class UImage;
struct FItemDefinition;

DECLARE_DELEGATE_OneParam(FOnProxyStatusChanged, bool bProxyExists);

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridItem : public UGSWidgetBase
{
	GENERATED_BODY()

public:
	void ConstructItem(const FItemDefinition& Def, float inSlotSize);

	FORCEINLINE FItemSize GetGridSize() const { return ItemSize; }

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	FOnProxyStatusChanged OnProxyStatusChanged;
	
protected:
	UPROPERTY(EditAnywhere)
    TSubclassOf<UGSGridItemProxy> ItemProxyClass;
	
private:
	void SetItemSize(const FItemDefinition& Def);
	void SetItemImage(const FItemDefinition& Def);

	void CreateItemProxy();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UImage> AltImage;
	
	UPROPERTY()
	TObjectPtr<UGSGridItemProxy> ItemProxy;

	FItemSize ItemSize;
	float SlotSize = 0.f;
};

