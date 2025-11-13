// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/GSEquipmentComponent.h"

#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/GSEquipItemActor.h"
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

	OwningCharacter = Cast<AGSPlayerCharacterBase>(GetOwner());
	InventoryComponent = UGSInventoryComponent::FindInventoryComponent(OwningCharacter.Get());
	if (OwningCharacter.IsValid())
	{
		OwnerSkeletalMesh = OwningCharacter->GetMesh();
	}

	InitInventoryComponent();
}

void UGSEquipmentComponent::InitInventoryComponent()
{
	if (InventoryComponent.IsValid())
	{
		InventoryComponent.Get()->TryEquipItemDelegate.BindUObject(this, &UGSEquipmentComponent::TryEquipItem);
		InventoryComponent.Get()->UnequipItemDelegate.BindUObject(this, &UGSEquipmentComponent::UnequipItem);
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
	
	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.GetFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnEquip(OwningCharacter.Get());
        AGSEquipItemActor* SpawnedActor = SpawnEquippedActor(EquipmentFragment);
        EquippedActors.Add(ItemDefinition.Type.RequestDirectParent(), SpawnedActor);
		return true;
	}
	return false;
}

bool UGSEquipmentComponent::CheckIfCanEquipItem(const FItemDefinition& ItemDefinition)
{
	// Check if there is already equipped item
	if (ItemDefinition.Type.MatchesTag(GSItemTags::Type::Weapon.GetTag()) || ItemDefinition.Type.MatchesTag(GSItemTags::Type::Armor.GetTag()))
	{
		if (EquippedActors.Contains(ItemDefinition.Type.RequestDirectParent()))
		{
			// TODO: switch weapon if can
			return false;
		}	
	}
	else if (EquippedActors.Contains(ItemDefinition.Type))
	{
		// TODO: switch weapon if can
		return false;
	}
	
	return true;
}

void UGSEquipmentComponent::UnequipItem(FItemInstance* UnequippedItem)
{
	if (!UnequippedItem)
	{
		return;
	}
	
	FItemDefinition& ItemDefinition = UnequippedItem->GetItemDefinitionMutable();
	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.GetFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnUnequip(OwningCharacter.Get());        	
        RemoveEquippedActor(ItemDefinition.Type.RequestDirectParent(), EquipmentFragment);
	}
}

AGSEquipItemActor* UGSEquipmentComponent::SpawnEquippedActor(FEquipmentFragment* EquipmentFragment) const
{
	AGSEquipItemActor* SpawnedActor = EquipmentFragment->SpawnEquipmentActor(OwnerSkeletalMesh.Get());
	//
	//	
	return SpawnedActor;
}

void UGSEquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipType, FEquipmentFragment* EquipmentFragment)
{
	UE_LOG(LogTemp, Warning, TEXT("TEXT"));
	EquippedActors.Remove(EquipType);
	EquipmentFragment->DestroyEquippedActor();
}
