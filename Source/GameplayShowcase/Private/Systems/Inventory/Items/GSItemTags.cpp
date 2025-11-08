// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemTags.h"


namespace GSItemTags
{
	namespace Type
	{
		namespace Weapon
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(OneHanded, "Item.Type.Weapon.OneHanded", "XXX.");
            UE_DEFINE_GAMEPLAY_TAG_COMMENT(TwoHanded, "Item.Type.Weapon.TwoHanded", "XXX.");
            UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dagger, "Item.Type.Weapon.Dagger", "XXX.");
		}
		
		namespace Armor
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Heavy, "Item.Type.Armor.Heavy", "XXX.");
		}
		
	}

	namespace GameItems
	{	
		namespace Weapons::OneHanded
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword1, "GameItems.Weapon.OneHanded.Sword1", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword2, "GameItems.Weapon.OneHanded.Sword2", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword3, "GameItems.Weapon.OneHanded.Sword3", "XXX.");
		}
		
	}	
}