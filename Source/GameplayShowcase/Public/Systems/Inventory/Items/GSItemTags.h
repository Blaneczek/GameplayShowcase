// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once


#include "NativeGameplayTags.h"
#include "CoreMinimal.h"

/**
 * Item Tags
 *
 */

namespace GSItemTags
{
	namespace Type
	{
		namespace Weapon
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(OneHanded);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(TwoHanded);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dagger);
		}
		
		namespace Armor
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Heavy);
		}
		
	}

	namespace GameItems
	{	
		namespace Weapons::OneHanded
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword1);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword2);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword3);
		}
		
	}	
}	
