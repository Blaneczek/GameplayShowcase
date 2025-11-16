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

	for (auto& Pair : AS->TagsToSingleAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	for (auto& Pair : AS->TagsToPairAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value.Key());
		BroadcastAttributeInfo(Pair.Key, Pair.Value.Value(), true);
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
	if (!AS || !AbilitySystemComponent || !AttributeInfo)
	{
		return;
	}

	// Bind Attributes Info
	for (auto& Pair : AS->TagsToSingleAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	for (auto& Pair : AS->TagsToPairAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value.Key()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value.Key());
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value.Value(), true);
			}
		);
	}

	if (UGSLevelingComponent* LevelingComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		LevelingComponent->OnLevelUpDelegate.AddLambda([this](int NewLevel) 
		{
			Level = NewLevel;
			OnLevelChanged.Broadcast(NewLevel);
		});
		LevelingComponent->OnXPChangedDelegate.AddLambda([this](const FCurrentLevelInfo& Info, int32 NewLevelsNum) 
		{
			CheckIfNewUpgradePoint(Info.XPPercent, NewLevelsNum);
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
		if (!AS->TagsToSingleAttributes.Contains(AttributeTag))
		{
			return;
		}
		if (const auto FuncPtr = AS->TagsToSingleAttributes.Find(AttributeTag); AbilitySystemComponent)
		{
			AbilitySystemComponent->ApplyModToAttribute((*FuncPtr)(), EGameplayModOp::AddFinal, 1.f);
			
			UpgradePointsAvailable--;
			OnAttributePointDelegate.Broadcast(UpgradePointsAvailable);
		}
	}
}

void UGSCharacterMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute, bool bIsMax) const
{
	if (FAttributeInfo* Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag))
	{
		bIsMax ? Info->AttributeValueMax = Attribute.GetNumericValue(AttributeSet) : Info->AttributeValue = Attribute.GetNumericValue(AttributeSet);
        AttributeInfoDelegate.Broadcast(*Info);
	}	
}

void UGSCharacterMenuWidgetController::CheckIfNewUpgradePoint(float NewPercent, int32 NewLevelsNum)
{
	const uint8 a = LevelPercent / 25;
    const uint8 b = NewPercent / 25;

	if (NewLevelsNum == 0 && b > a)
	{
		UpgradePointsAvailable += (b-a);   
	}
	else if (NewLevelsNum > 0)
	{
		// 3 points per level
		// (3 - a) - from current level, b - from percent of new level, (3 * (NewLevelsNum - 1)) - from all new levels minus current level 
		UpgradePointsAvailable += ( (3 - a) + b + (3 * (NewLevelsNum - 1)) );
	}

	OnAttributePointDelegate.Broadcast(UpgradePointsAvailable);
	LevelPercent = NewPercent;
}
