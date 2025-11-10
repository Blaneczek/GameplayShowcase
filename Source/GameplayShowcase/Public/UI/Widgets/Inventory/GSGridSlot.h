// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "GSGridSlot.generated.h"

class UBorder;

DECLARE_DELEGATE_TwoParams(FCheckAllItemPositionsSignature, const FGridPosition& Position, const FItemSize& inProxySize);
DECLARE_DELEGATE(FClearAllItemPositionsSignature);

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSGridSlot : public UUserWidget
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetOccupancyStatus(bool bIsOccupied);	
	FORCEINLINE bool IsOccupied() const { return bOccupied; };

	FORCEINLINE FGridPosition GetPosition() const { return Position; }; 
	FORCEINLINE void SetPosition(int32 RowIndex, int32 ColumnIndex)
	{
		Position = FGridPosition(RowIndex, ColumnIndex);
	};
	
	void SetItemProxyStatus(bool bProxyExists, const FItemSize& inProxySize);
	void SetHoveredColor(bool bCanRelocateItem);
	void ClearSlot();
	
	FCheckAllItemPositionsSignature CheckAllItemPositionsDelegate;
	FClearAllItemPositionsSignature ClearAllItemsPositionsDelegate;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(EditAnywhere)
	FLinearColor DefaultColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere)
	FLinearColor RelocationForbiddenColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere)
	FLinearColor RelocationAllowedColor = FLinearColor::White;
	
private:	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> GridSlot;

	TArray<FGridPosition> PositionsToClear;
	
	FGridPosition Position;
	FItemSize ProxySize;
	int32 InventoryGridIndex = 0;
	
	bool bOccupied = false;
	bool bItemProxyExists = false;
	bool bRelocationAllowed = false;
};
