// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GSLevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FLevelUpInfo
{
	GENERATED_BODY()

	FLevelUpInfo() = default;

	UPROPERTY(EditDefaultsOnly)
	int32 Level = 1;
	
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointReward = 1;
};

/**
 *
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSLevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FLevelUpInfo> LevelUpInformation;

	int32 FindLevelForXP(int32 XP) const;
	int32 FindMinXPForLevel(int32 Level) const;
};
