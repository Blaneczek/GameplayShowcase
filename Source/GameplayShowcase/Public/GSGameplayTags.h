// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "NativeGameplayTags.h"
#include "CoreMinimal.h"

/**
 * GSGameplayTags
 *
 */
namespace GSGameplayTags
{
	/** ATTRIBUTES **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Character_VIT);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Character_INT);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Character_STR);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Character_DEX);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_HP);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MaxHP);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_HPRegen);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_PE);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MaxPE);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_PERegen);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_ST);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MaxST);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_STRegen);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_AttackDamage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MagicDamage);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_Defence);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MagicDefence);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_Evading);
	/***************/
};

