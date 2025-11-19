// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/GSEquipmentComponent.h"

#include "GSBlueprintFunctionLibrary.h"
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
	if (UGSInventoryComponent* InvComponent = InventoryComponent.Get())
	{
		InvComponent->TryEquipItemDelegate.BindUObject(this, &UGSEquipmentComponent::TryEquipItem);
		InvComponent->UnequipItemDelegate.BindUObject(this, &UGSEquipmentComponent::UnequipItem);
	}
}

bool UGSEquipmentComponent::TryEquipItem(FItemInstance* EquippedItem)
{
	FItemDefinition& ItemDefinition = EquippedItem->GetItemDefinitionMutable();
	if (!CheckIfCanEquipItem(ItemDefinition))
	{
		return false;
	}
	
	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.GetFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnEquip(OwningCharacter.Get());
		// SpawnEquippedActor can return nullptr, it means that equipped item doesn't have in word representation,
		// but we need to add it to EquippedActors to check later if this type of item is equipped.
		AGSEquipItemActor* SpawnedActor = SpawnEquippedActor(EquipmentFragment);
		EquippedActors.Add(ItemDefinition.Type.RequestDirectParent(), SpawnedActor);
		return true;
	}
	return false;
}

bool UGSEquipmentComponent::CheckIfCanEquipItem(const FItemDefinition& ItemDefinition)
{
	// Check if Player has required Level
	if (const AGSPlayerCharacterBase* PlayerChar = OwningCharacter.Get())
	{
		if (ItemDefinition.Level > PlayerChar->GetPlayerLevel())
		{
			return false;
		}
	}
	
	// Check if there is already equipped item
	const FGameplayTag ItemType = ItemDefinition.Type;
	FGameplayTag TagToCheck = ItemType;
	if (ItemDefinition.Type.MatchesTag(GSItemTags::Type::Weapon.GetTag())
		|| ItemDefinition.Type.MatchesTag(GSItemTags::Type::Armor.GetTag()))
	{
		TagToCheck = ItemType.RequestDirectParent();
	}	
	if (EquippedActors.Contains(TagToCheck))
	{
		return false;
	}
	return true;
}

void UGSEquipmentComponent::UnequipItem(FItemInstance* UnequippedItem)
{
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
	return SpawnedActor;
}

void UGSEquipmentComponent::RemoveEquippedActor(const FGameplayTag& EquipType, FEquipmentFragment* EquipmentFragment)
{
	EquippedActors.Remove(EquipType);
	EquipmentFragment->DestroyEquippedActor();
}
