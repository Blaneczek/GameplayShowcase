// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GSAttributeInfo.generated.h"

struct FGameplayTag;

USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()

	FAttributeInfo() {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();
	
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;

	/* Optional if there is an additional Attribute that specifies the maximum value, e.g. HP - MaxHP, AttackDamageMin - AttackDamageMax  */
	UPROPERTY(BlueprintReadOnly)
	float AttributeValueMax = 0.f;	
};

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAttributeInfo* FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAttributeInfo> AttributeInformation;
};
