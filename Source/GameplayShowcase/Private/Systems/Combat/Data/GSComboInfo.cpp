// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/Data/GSComboInfo.h"

FComboAction UGSComboInfo::GetComboAction(int32 ComboIndex) const
{
	if (!ComboActions.IsValidIndex(ComboIndex))
	{
		return FComboAction();
	}

	return ComboActions[ComboIndex];
}

bool UGSComboInfo::HasValidCombo() const
{
	if (ComboActions.IsEmpty())
	{
		return false;
	}
	
	for (const auto& ComboAction : ComboActions)
	{
		if (!ComboAction.IsValid())
		{
			return false;
		}
	}
	return true;
}
