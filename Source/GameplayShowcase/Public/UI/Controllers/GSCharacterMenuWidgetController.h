// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "GSCharacterMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
class UGSAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpgradePointSignature, int32, NumberOfAvailablePoints);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYSHOWCASE_API UGSCharacterMenuWidgetController : public UGSWidgetControllerBase
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature UpgradePointDelegate;
	
	//void AddNewUpgradePoint();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGSAttributeInfo> AttributeInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 UpgradePointsAvailable = 0;
	
private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
