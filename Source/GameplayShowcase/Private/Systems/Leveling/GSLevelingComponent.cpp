// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Leveling/GSLevelingComponent.h"

#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "Systems/Leveling/GSLevelUpInfo.h"


UGSLevelingComponent::UGSLevelingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

UGSLevelingComponent* UGSLevelingComponent::FindLevelingComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSLevelingComponent>() : nullptr;
}

void UGSLevelingComponent::SetLevel(int32 NewLevel)
{
	const int32 NewLevelsNum = NewLevel - Level;
	Level = FMath::Clamp(NewLevel, 1, LevelUpInfo->LevelUpInformation.Num());
	OnLevelChangedDelegate.Broadcast(Level);
	
	// Set min XP for given level
	const int32 NewXP = LevelUpInfo->FindMinXPForLevel(Level);
	if (AttributeSet)
	{
		AttributeSet.Get()->SetXP(NewXP);
		OnXPChangedDelegate.Broadcast(GetCurrentLevelInfo(), NewLevelsNum);
	}
}

void UGSLevelingComponent::AddToXP(int32 InXP)
{
	if (AttributeSet)
	{
		const int32 NewXP = AttributeSet.Get()->GetXP() + InXP;
		const int32 NewLevel = LevelUpInfo->FindLevelForXP(NewXP);
		const int32 NumLevelUps = NewLevel - Level;
		if (NumLevelUps > 0)
		{
			LevelUp(NumLevelUps);
		}	
		AttributeSet.Get()->SetXP(NewXP);

		// Each time we add xp, we update UI info so we need information about current level
		OnXPChangedDelegate.Broadcast(GetCurrentLevelInfo(), NumLevelUps);
	}
}

void UGSLevelingComponent::LevelUp(int32 InLevels)
{
	Level += InLevels;
	OnLevelChangedDelegate.Broadcast(Level);
}


int32 UGSLevelingComponent::GetLevel() const
{
	return Level;
}

int32 UGSLevelingComponent::GetXP() const
{
	if (AttributeSet)
	{
		return AttributeSet.Get()->GetXP();
	}
	return 0;
}

FCurrentLevelInfo UGSLevelingComponent::GetCurrentLevelInfo() const
{
	if (!LevelUpInfo || LevelUpInfo->LevelUpInformation.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelUpInfo is not valid"));
		return FCurrentLevelInfo();
	}
	
	const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;
	const int32 PreviousLevelUpRequirement = Level == 1 ? 0 : LevelUpInfo->LevelUpInformation[Level - 2].LevelUpRequirement;
	const int32 CurrentLevelXP = GetXP() - PreviousLevelUpRequirement;
	const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
	const float Percent = (static_cast<float>(CurrentLevelXP) / static_cast<float>(DeltaLevelRequirement)) * 100.f;
	
	return FCurrentLevelInfo(CurrentLevelXP, DeltaLevelRequirement, Percent);
}

void UGSLevelingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AGSPlayerCharacterBase* OwnerChar = Cast<AGSPlayerCharacterBase>(GetOwner()))
	{
		AttributeSet = Cast<UGSAttributeSetPlayer>(OwnerChar->GetAttributeSet());
	}
}



