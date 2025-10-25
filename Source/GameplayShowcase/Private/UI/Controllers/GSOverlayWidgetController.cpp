// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSOverlayWidgetController.h"
#include "AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"

void UGSOverlayWidgetController::BroadcastInitialValues()
{
	if (const UGSAttributeSetPlayer* ASPlayer = Cast<UGSAttributeSetPlayer>(AttributeSet))
	{
		OnHPChanged.Broadcast(ASPlayer->GetHP());
        OnMaxHPChanged.Broadcast(ASPlayer->GetMaxHP());	
        OnPEChanged.Broadcast(ASPlayer->GetPE());
        OnMaxPEChanged.Broadcast(ASPlayer->GetMaxPE());
		OnSTChanged.Broadcast(ASPlayer->GetST());
		OnMaxSTChanged.Broadcast(ASPlayer->GetMaxST());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGSOverlayWidgetController::BroadcastInitialValues | ASPlayer is not valid"));
	}
}

void UGSOverlayWidgetController::BindCallbacksToDependencies()
{
	const UGSAttributeSetPlayer* ASPlayer = Cast<UGSAttributeSetPlayer>(AttributeSet);
	if (!ASPlayer || !AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGSOverlayWidgetController::BindCallbacksToDependencies | ASPlayer or AbilitySystemComponent is not valid"));
		return;
	}
		
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ASPlayer->GetHPAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHPChanged.Broadcast(Data.NewValue);
			});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ASPlayer->GetMaxHPAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHPChanged.Broadcast(Data.NewValue);
			});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ASPlayer->GetPEAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnPEChanged.Broadcast(Data.NewValue);
			});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ASPlayer->GetMaxPEAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxPEChanged.Broadcast(Data.NewValue);
			});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ASPlayer->GetSTAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnSTChanged.Broadcast(Data.NewValue);
			});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		ASPlayer->GetMaxSTAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxSTChanged.Broadcast(Data.NewValue);
			});	
}