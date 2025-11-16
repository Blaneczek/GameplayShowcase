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
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_OneHanded);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_TwoHanded);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Weapon_Dagger);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor_00);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor_01);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor_Heavy);	
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor_Medium);	
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor_Light);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Helmet);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shield);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Belt);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boots);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Bracelet);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Earrings);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Necklace);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Potion);
	}

	namespace GameItems
	{	
		namespace Weapons::OneHanded
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sword);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(LongSword);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(CrescentSword);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(BambooSword);
		}
		namespace Armor::Heavy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MonkPlate);
		}
		namespace Potion::HP
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedPotionS);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedPotionM);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(RedPotionL);
		}
	}	
}	
