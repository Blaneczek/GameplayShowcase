// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemTags.h"


namespace GSItemTags
{
	namespace Type
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon, "Item.Type.Weapon", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_OneHanded, "Item.Type.Weapon.One-handed Sword", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_TwoHanded, "Item.Type.Weapon.Two-handed Sword", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Dagger, "Item.Type.Weapon.Dagger", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Bow, "Item.Type.Weapon.Bow", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Fan, "Item.Type.Weapon.Fan", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Weapon_Bell, "Item.Type.Weapon.Bell", "XXX.");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "Item.Type.Armor", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_00, "Item.Type.Armor.00", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_01, "Item.Type.Armor.01", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_Heavy, "Item.Type.Armor.Heavy Armor", "XXX.");		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_Medium, "Item.Type.Armor.Medium Armor", "XXX.");		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor_Light, "Item.Type.Armor.Light Armor", "XXX.");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Helmet, "Item.Type.Helmet", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Shield, "Item.Type.Shield", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Boots, "Item.Type.Boots", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Bracelet, "Item.Type.Bracelet", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Earrings, "Item.Type.Earrings", "XXX.");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Necklace, "Item.Type.Necklace", "XXX.");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Potion, "Item.Type.Potion", "XXX.");
	}

	namespace GameItems
	{	
		namespace Weapons::OneHanded
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "GameItems.Weapon.OneHanded.Sword", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(LongSword, "GameItems.Weapon.OneHanded.Long Sword", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(CrescentSword, "GameItems.Weapon.OneHanded.Crescent Sword", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(BambooSword, "GameItems.Weapon.OneHanded.Bamboo Sword", "XXX.");
		}
		namespace Armor::Heavy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MonkPlate, "GameItems.Armor.Heavy.Monk Plate Armor", "XXX.");
		}
		namespace Potion::HP
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(RedPotionS, "GameItems.Potion.HP.Red Potion(S)", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(RedPotionM, "GameItems.Potion.HP.Red Potion(M)", "XXX.");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(RedPotionL, "GameItems.Potion.HP.Red Potion(L)", "XXX.");
		}
	}	
}