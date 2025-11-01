// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSOverlayWidgetController.h"

#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "Systems/Leveling/GSLevelingComponent.h"

void UGSOverlayWidgetController::BroadcastInitialValues()
{
	if (const UGSAttributeSetPlayer* AS = Cast<UGSAttributeSetPlayer>(AttributeSet))
	{
		OnHPChanged.Broadcast(AS->GetHP());
        OnMaxHPChanged.Broadcast(AS->GetMaxHP());	
        OnPEChanged.Broadcast(AS->GetPE());
        OnMaxPEChanged.Broadcast(AS->GetMaxPE());
		OnSTChanged.Broadcast(AS->GetST());
		OnMaxSTChanged.Broadcast(AS->GetMaxST());
	}

	if (const UGSLevelingComponent* LevelingComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		OnXPPercentChangedDelegate.Broadcast(LevelingComponent->GetCurrentLevelInfo().XPPercent);
	}
}

void UGSOverlayWidgetController::BindCallbacksToDependencies()
{
	const UGSAttributeSetPlayer* AS = Cast<UGSAttributeSetPlayer>(AttributeSet);
	UAbilitySystemComponent* ASC = AbilitySystemComponent;
	if (!AS || !ASC)
	{
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetHPAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHPChanged.Broadcast(Data.NewValue);
			});
	
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetMaxHPAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHPChanged.Broadcast(Data.NewValue);
			});

	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetPEAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnPEChanged.Broadcast(Data.NewValue);
			});
	
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetMaxPEAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxPEChanged.Broadcast(Data.NewValue);
			});

	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetSTAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnSTChanged.Broadcast(Data.NewValue);
			});
	
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetMaxSTAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxSTChanged.Broadcast(Data.NewValue);
			});

	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetMaxSTAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxSTChanged.Broadcast(Data.NewValue);
			});

	if (UGSLevelingComponent* LevelingComponent = UGSLevelingComponent::FindLevelingComponent(Character))
	{
		LevelingComponent->OnXPChangedDelegate.AddLambda([this](const FCurrentLevelInfo& Info) 
		{
			OnXPPercentChangedDelegate.Broadcast(Info.XPPercent);
		});
	}
}

