// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSWidgetControllerBase.h"
#include "GSCharacterMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
class UGSAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentLevelXPChangedSignature, int32, XP, int32, MaxXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLevelChangedSignature, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributePointSignature, int32, NumOfAvailablePoints);

using FAttributePair = TPair<FGameplayAttribute(*)(), FGameplayAttribute(*)()>;

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

protected:
	UFUNCTION(BlueprintCallable, Category = "GAS|Attributes")
	void SpendAttributePoint(const FGameplayTag& AttributeTag);
	
private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute, bool bIsMax = false) const;

	/* New upgrade point after 25%, 50%, 75% */
	void CheckIfNewUpgradePoint(float NewPercent, int32 NewLevelsNum);
	
public:
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="GS|Leveling")
	FOnPlayerLevelChangedSignature OnLevelChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GS|Leveling")
	FOnCurrentLevelXPChangedSignature OnXPChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GS|Leveling")
	FAttributePointSignature OnAttributePointDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGSAttributeInfo> AttributeInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 UpgradePointsAvailable = 0;
	
private:
	int32 Level = 1;
	int32 LevelsDelta = 0;
	float LevelPercent = 0.0f;
	
};
