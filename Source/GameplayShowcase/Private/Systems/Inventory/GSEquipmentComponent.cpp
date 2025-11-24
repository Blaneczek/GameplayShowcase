// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/GSEquipmentComponent.h"

#include "GSBlueprintFunctionLibrary.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/GSItemInstance.h"
#include "Systems/Inventory/Items/GSItemTags.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"


UGSEquipmentComponent::UGSEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGSEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerChar = Cast<AGSPlayerCharacterBase>(GetOwner());
	if (AGSPlayerCharacterBase* OwnerChar = CachedOwnerChar.Get())
	{
		CachedInventoryComponent = UGSInventoryComponent::FindInventoryComponent(OwnerChar);
		CachedOwnerSkeletalMesh = OwnerChar->GetMesh();
	}
	
	BindToInventoryComponent();
}

void UGSEquipmentComponent::BindToInventoryComponent()
{
	if (UGSInventoryComponent* InvComponent = CachedInventoryComponent.Get())
	{
		InvComponent->TryEquipItemDelegate.BindUObject(this, &UGSEquipmentComponent::TryEquipItem);
		InvComponent->UnequipItemDelegate.BindUObject(this, &UGSEquipmentComponent::UnequipItem);
	}
}

bool UGSEquipmentComponent::TryEquipItem(FItemInstance* EquippedItem)
{
	if (!EquippedItem)
	{
		return false;
	}
	
	FItemDefinition& ItemDefinition = EquippedItem->GetItemDefinitionMutable();
	if (!CheckIfCanEquipItem(ItemDefinition))
	{
		return false;
	}
	
	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.FindFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnEquip(CachedOwnerChar.Get());
		// SpawnEquippedActor can return nullptr, it means that equipped item doesn't have in word representation,
		// but we need to add it to EquippedActors to check later if this type of item is equipped.
		AGSEquipItemActor* SpawnedActor = SpawnEquippedActor(EquipmentFragment);
		EquippedActors.Add(ItemDefinition.Type.RequestDirectParent(), SpawnedActor);
		return true;
	}
	return false;
}

bool UGSEquipmentComponent::CheckIfCanEquipItem(const FItemDefinition& ItemDefinition) const
{
	// Validate level requirement
	if (const AGSPlayerCharacterBase* PlayerChar = CachedOwnerChar.Get())
	{
		if (ItemDefinition.Level > PlayerChar->GetPlayerLevel())
		{
			return false;
		}
	}
	
	// Check if there is already equipped item
	const FGameplayTag ItemType = ItemDefinition.Type;
	FGameplayTag SlotToCheck = ItemType;

	// Weapons and Armors have additional subcategory (one-handed, heavy etc.)
	if (ItemDefinition.Type.MatchesTag(GSItemTags::Type::Weapon.GetTag())
		|| ItemDefinition.Type.MatchesTag(GSItemTags::Type::Armor.GetTag()))
	{
		SlotToCheck = ItemType.RequestDirectParent();
	}

	return !EquippedActors.Contains(SlotToCheck);
}

void UGSEquipmentComponent::UnequipItem(FItemInstance* UnequippedItem)
{
	if (!UnequippedItem)
	{
		return;
	}
	
	FItemDefinition& ItemDefinition = UnequippedItem->GetItemDefinitionMutable();
	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.FindFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnUnequip(CachedOwnerChar.Get());        	
        RemoveEquippedActor(ItemDefinition.Type.RequestDirectParent(), EquipmentFragment);
	}
}

AGSEquipItemActor* UGSEquipmentComponent::SpawnEquippedActor(FEquipmentFragment* EquipmentFragment)
{
	AGSEquipItemActor* SpawnedActor = EquipmentFragment->SpawnEquipmentActor(CachedOwnerSkeletalMesh.Get());
	return SpawnedActor;
}

void UGSEquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipType, FEquipmentFragment* EquipmentFragment)
{
	EquippedActors.Remove(EquipType);
	EquipmentFragment->DestroyEquippedActor();
}
