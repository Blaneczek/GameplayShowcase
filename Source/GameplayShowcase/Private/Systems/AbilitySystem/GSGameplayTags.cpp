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
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_VIT, "Attribute.Character.Vitality", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_INT, "Attribute.Character.Intelligence", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_STR, "Attribute.Character.Strength", "XXX.");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_DEX, "Attribute.Character.Dexterity", "XXX.");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_HP, "Attribute.Primary.HP", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MaxHP, "Attribute.Primary.MaxHP", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_HPRegen, "Attribute.Primary.HP Regeneration", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_PE, "Attribute.Primary.PE", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MaxPE, "Attribute.Primary.MaxPE", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_PERegen, "Attribute.Primary.PE Regeneration", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_ST, "Attribute.Primary.Stamina", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MaxST, "Attribute.Primary.MaxST", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_STRegen, "Attribute.Primary.Stamina Regeneration", "XXX.");
        	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackDamage, "Attribute.Primary.Attack Damage", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackDamageMin, "Attribute.Primary.AttackDamageMin", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackDamageMax, "Attribute.Primary.AttackDamageMax", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDamage, "Attribute.Primary.Magic Damage", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDamageMin, "Attribute.Primary.MagicDamageMin", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDamageMax, "Attribute.Primary.MagicDamageMax", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_AttackSpeed, "Attribute.Primary.AttackSpeed", "XXX.");
		
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_Defence, "Attribute.Primary.Defence", "XXX.");
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_MagicDefence, "Attribute.Primary.Magic Defence", "XXX.");     	
        UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_Evading, "Attribute.Primary.Evading", "XXX.");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Others_XP, "Attribute.Others.XP", "XXX.");
	}

	
	/***************/

	/** Statuses **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_ST_Regen, "Status.ST.Regen", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_ST_Consuming, "Status.ST.Consuming", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_ST_Zero, "Status.ST.Zero", "XXX.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_HP_Regen, "Status.HP.Regen", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_HP_Consuming, "Status.HP.Consuming", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_HP_Zero, "Status.HP.Zero", "XXX.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_PE_Regen, "Status.PE.Regen", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_PE_Consuming, "Status.PE.Consuming", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_PE_Zero, "Status.PE.Zero", "XXX.");

	/** Ability **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_GA_Sprint, "Ability.GA.Sprint", "XXX.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Requires_ST, "Ability.Requires.ST", "XXX.");

	/** EVENTS **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Character_Movement_Stopped, "Event.Character.Movement.Stopped", "XXX.");
}
