// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "GSOverlayWidgetController.generated.h"


class UCanvasPanel;
class UGSLevelingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeChangedSignature, float, NewValue);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYSHOWCASE_API UGSOverlayWidgetController : public UGSWidgetControllerBase
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	UCanvasPanel* GetCanvasPanelRef() const { return CanvasRef.Get(); }
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable)
	void SetCanvasPanelRef(UCanvasPanel* CanvasPanel);
	
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

	TWeakObjectPtr<UCanvasPanel> CanvasRef;
};
