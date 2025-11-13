// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemTags.h"


namespace GSItemTags
{
	namespace Type
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon, "Item.Type.Weapon", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_OneHanded, "Item.Type.Weapon.OneHanded", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_TwoHanded, "Item.Type.Weapon.TwoHanded", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Dagger, "Item.Type.Weapon.Dagger", "XXX.");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "Item.Type.Armor", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_Heavy, "Item.Type.Armor.Heavy", "XXX.");		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_Medium, "Item.Type.Armor.Medium", "XXX.");		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_Light, "Item.Type.Armor.Light", "XXX.");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Helmet, "Item.Type.Helmet", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shield, "Item.Type.Shield", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Boots, "Item.Type.Boots", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bracelet, "Item.Type.Bracelet", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Earrings, "Item.Type.Earrings", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Necklace, "Item.Type.Necklace", "XXX.");	
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