// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "GSOverlayWidgetController.generated.h"


class UCanvasPanel;
class UGSLevelingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUpMessageAddedSignature, const FText&, Message);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYSHOWCASE_API UGSOverlayWidgetController : public UGSWidgetControllerBase
{
	GENERATED_BODY()


public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable)
	void SetCanvasPanelRef(UCanvasPanel* CanvasPanel);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCanvasPanel* GetCanvasPanelRef() const { return CanvasRef.Get(); }

	void OnItemPickUpMessage(const FText& Message);
	
	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnMaxHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnPEChanged;

	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnMaxPEChanged;

	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnSTChanged;

	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnMaxSTChanged;

	UPROPERTY(BlueprintAssignable, Category = "GS|Attributes")
	FAttributeChangedSignature OnXPPercentChangedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "GS|Inventory")
	FOnPickUpMessageAddedSignature OnPickUpMessageAdded;
	
	TWeakObjectPtr<UCanvasPanel> CanvasRef;
};
