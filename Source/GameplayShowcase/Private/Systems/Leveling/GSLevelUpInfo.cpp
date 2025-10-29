// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#include "Systems/Leveling/GSLevelUpInfo.h"

int32 UGSLevelUpInfo::FindLevelForXP(int32 XP)
{
	for (const auto& Info : LevelUpInfo)
	{
		if (XP < Info.LevelUpRequirement && XP > 0)
		{
			return Info.Level;
		}
	}

	return 1;
}
