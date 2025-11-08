// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/AbilitySystem/GSGameplayTags.h"

namespace GSGameplayTags
{
	/** INPUT **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_LeftShift, "Input.LeftShift", "XXX.");
	/***************/
	
	namespace Attributes
	{
		/* Character Attributes */
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_VIT, "Attribute.Character.VIT", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_INT, "Attribute.Character.INT", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_STR, "Attribute.Character.STR", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_DEX, "Attribute.Character.DEX", "XXX.");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_HP, "Attribute.Primary.HP", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MaxHP, "Attribute.Primary.MaxHP", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_HPRegen, "Attribute.Primary.HPRegen", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_PE, "Attribute.Primary.PE", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MaxPE, "Attribute.Primary.MaxPE", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_PERegen, "Attribute.Primary.PERegen", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_ST, "Attribute.Primary.ST", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MaxST, "Attribute.Primary.MaxST", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_STRegen, "Attribute.Primary.STRegen", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackDamage, "Attribute.Primary.AttackDamage", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackDamageMin, "Attribute.Primary.AttackDamageMin", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackDamageMax, "Attribute.Primary.AttackDamageMax", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDamage, "Attribute.Primary.MagicDamage", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDamageMin, "Attribute.Primary.MagicDamageMin", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDamageMax, "Attribute.Primary.MagicDamageMax", "XXX.");
  	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_Defence, "Attribute.Primary.Defence", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDefence, "Attribute.Primary.MagicDefence", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_Evading, "Attribute.Primary.Evading", "XXX.");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Others_XP, "Attribute.Others.XP", "XXX.");
	}

	
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
