// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "GSGameplayTags.h"

namespace GSGameplayTags
{
	/** INPUT **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_LeftShift, "Input.LeftShift", "XXX.");
	/***************/
	
	/** ATTRIBUTES **/
	/* Character Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Character_VIT, "Attribute.Character.VIT", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Character_INT, "Attribute.Character.INT", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Character_STR, "Attribute.Character.STR", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Character_DEX, "Attribute.Character.DEX", "XXX.");
	
	/* Primary Attributes */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_HP, "Attribute.Primary.HP", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MaxHP, "Attribute.Primary.MaxHP", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_HPRegen, "Attribute.Primary.HPRegen", "XXX.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_PE, "Attribute.Primary.PE", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MaxPE, "Attribute.Primary.MaxPE", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_PERegen, "Attribute.Primary.PERegen", "XXX.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_ST, "Attribute.Primary.ST", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MaxST, "Attribute.Primary.MaxST", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_STRegen, "Attribute.Primary.STRegen", "XXX.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_AttackDamage, "Attribute.Primary.AttackDamage", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MagicDamage, "Attribute.Primary.MagicDamage", "XXX.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_Defence, "Attribute.Primary.Defence", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MagicDefence, "Attribute.Primary.MagicDefence", "XXX.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_Evading, "Attribute.Primary.Evading", "XXX.");
	/***************/

	/** Statuses **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_ST_Regen, "Status.ST.Regen", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_ST_Consuming, "Status.ST.Consuming", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_ST_Zero, "Status.ST.Zero", "XXX.");

	/** Ability **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_GA_Sprint, "Ability.GA.Sprint", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Requires_ST, "Ability.Requires.ST", "XXX.");

	/** EVENTS **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Character_Movement_Stopped, "Event.Character.Movement.Stopped", "XXX.");
}
