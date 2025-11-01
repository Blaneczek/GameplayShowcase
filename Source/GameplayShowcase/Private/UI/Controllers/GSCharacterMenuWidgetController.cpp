// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSCharacterMenuWidgetController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "Systems/AbilitySystem/Data/GSAttributeInfo.h"
#include "Systems/Leveling/GSLevelingComponent.h"

void UGSCharacterMenuWidgetController::BroadcastInitialValues()
{
	const UGSAttributeSetPlayer* AS = Cast<UGSAttributeSetPlayer>(AttributeSet);
	if (!AS || !AttributeInfo)
	{
		return;
	}

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	if (const UGSLevelingComponent* LevelingComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		const FCurrentLevelInfo Info = LevelingComponent->GetCurrentLevelInfo();
		OnXPChanged.Broadcast(Info.XP, Info.MaxXP);
		OnLevelChanged.Broadcast(LevelingComponent->GetLevel());
	}

	OnAttributePointDelegate.Broadcast(UpgradePointsAvailable);
}

void UGSCharacterMenuWidgetController::BindCallbacksToDependencies()
{
	const UGSAttributeSetPlayer* AS = Cast<UGSAttributeSetPlayer>(AttributeSet);
	UAbilitySystemComponent* ASC = AbilitySystemComponent;
	if (!AS || !ASC || !AttributeInfo)
	{
		return;
	}

	// Bind Attributes Info
	for (auto& Pair : AS->TagsToAttributes)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	if (UGSLevelingComponent* LevelingComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		LevelingComponent->OnLevelUpDelegate.AddLambda([this](int NewLevel) 
		{
			LevelsDelta = NewLevel - Level;
			Level = NewLevel;
			OnLevelChanged.Broadcast(NewLevel);
		});
		LevelingComponent->OnXPChangedDelegate.AddLambda([this](const FCurrentLevelInfo& Info) 
		{
			CheckIfNewUpgradePoint(Info.XPPercent);
			OnXPChanged.Broadcast(Info.XP, Info.MaxXP);
		});
	}
}

void UGSCharacterMenuWidgetController::SpendAttributePoint(const FGameplayTag& AttributeTag)
{
	if (UpgradePointsAvailable < 1)
	{
		return;
	}
	
	if (const UGSAttributeSetPlayer* AS = Cast<UGSAttributeSetPlayer>(AttributeSet))
	{
		if (const auto FuncPtr = AS->TagsToAttributes.Find(AttributeTag); AbilitySystemComponent)
		{
			AbilitySystemComponent->ApplyModToAttribute((*FuncPtr)(), EGameplayModOp::AddFinal, 1.f);
			
			UpgradePointsAvailable--;
			OnAttributePointDelegate.Broadcast(UpgradePointsAvailable);
		}
	}
}

void UGSCharacterMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                              const FGameplayAttribute& Attribute) const
{
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UGSCharacterMenuWidgetController::CheckIfNewUpgradePoint(float NewPercent)
{
	const uint8 a = LevelPercent / 25;
    const uint8 b = NewPercent / 25;
        
     // New Upgrade point
    if (b > a)
    {
    	UpgradePointsAvailable += (b-a);
    	OnAttributePointDelegate.Broadcast(UpgradePointsAvailable);
    }

	LevelPercent = NewPercent;
}
