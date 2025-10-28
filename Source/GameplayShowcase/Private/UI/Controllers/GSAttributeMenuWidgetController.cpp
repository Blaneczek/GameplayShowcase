// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Controllers/GSAttributeMenuWidgetController.h"
#include "AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "AbilitySystem/Data/GSAttributeInfo.h"

void UGSAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UGSAttributeSetPlayer* ASPlayer = Cast<UGSAttributeSetPlayer>(AttributeSet.Get());
	if (!ASPlayer || !AttributeInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("UGSAttributeMenuWidgetController::BroadcastInitialValues | ASPlayer or AttributeInfo is not valid"));
		return;
	}

	for (auto& Pair : ASPlayer->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UGSAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UGSAttributeSetPlayer* ASPlayer = Cast<UGSAttributeSetPlayer>(AttributeSet.Get());
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASPlayer || !ASC || !AttributeInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("UGSAttributeMenuWidgetController::BindCallbacksToDependencies | ASPlayer or ASC or AttributeInfo is not valid"));
		return;
	}
	
	for (auto& Pair : ASPlayer->TagsToAttributes)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
}

void UGSAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet.Get());
	AttributeInfoDelegate.Broadcast(Info);
}