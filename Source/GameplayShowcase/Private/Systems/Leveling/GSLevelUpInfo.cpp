// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#include "Systems/Leveling/GSLevelUpInfo.h"

int32 UGSLevelUpInfo::FindLevelForXP(int32 XP) const 
{
	if (LevelUpInformation.IsEmpty() || XP <= 0)
	{
		return 1;
	}

	int32 Index = Algo::UpperBoundBy(LevelUpInformation, XP, [](const auto& Info) { return Info.LevelUpRequirement; });
	Index = FMath::Clamp<int32>(Index, 0, LevelUpInformation.Num() - 1);
	
	return LevelUpInformation[Index].Level;
}

int32 UGSLevelUpInfo::FindMinXPForLevel(int32 Level) const
{
	if (Level > 1 && Level <= LevelUpInformation.Num())
	{
		// "Level - 2" because LevelUpInformation[0] is Level 1, and we want previous LevelUpRequirement so -2
		return LevelUpInformation[Level - 2].LevelUpRequirement;
	}
	return 0;
}
