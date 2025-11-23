// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace GSFragmentTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentFragment)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ConsumableFragment)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StackableFragment)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GridFragment)

	namespace Modifiers
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Defence)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttackSpeed)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes)
	}
}
