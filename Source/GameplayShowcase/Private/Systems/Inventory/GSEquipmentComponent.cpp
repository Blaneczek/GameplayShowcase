// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/GSEquipmentComponent.h"

#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/GSEquipItemActor.h"
#include "Systems/Inventory/Items/GSItemInstance.h"
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
		InventoryComponent.Get()->OnItemEquipped.AddDynamic(this, &UGSEquipmentComponent::OnItemEquipped);
		InventoryComponent.Get()->OnItemUnequipped.AddDynamic(this, &UGSEquipmentComponent::OnItemUnequipped);
	}
}

void UGSEquipmentComponent::OnItemEquipped(FItemInstance& EquippedItem)
{
	FItemDefinition& ItemDefinition = EquippedItem.GetItemDefinitionMutable();

	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.GetFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnEquip(OwningCharacter.Get());
        AGSEquipItemActor* SpawnedActor = SpawnEquippedActor(EquipmentFragment);
        EquippedActors.Add(SpawnedActor);
	}	
}

void UGSEquipmentComponent::OnItemUnequipped(FItemInstance& UnequippedItem)
{
	FItemDefinition& ItemDefinition = UnequippedItem.GetItemDefinitionMutable();

	if (FEquipmentFragment* EquipmentFragment = ItemDefinition.GetFragmentByTypeMutable<FEquipmentFragment>())
	{
		EquipmentFragment->OnUnequip(OwningCharacter.Get());        	
        RemoveEquippedActor(EquipmentFragment);
	}
}

AGSEquipItemActor* UGSEquipmentComponent::SpawnEquippedActor(FEquipmentFragment* EquipmentFragment) const
{
	AGSEquipItemActor* SpawnedActor = EquipmentFragment->SpawnEquipmentActor(OwnerSkeletalMesh.Get());
	//
	//	
	return SpawnedActor;
}

void UGSEquipmentComponent::RemoveEquippedActor(FEquipmentFragment* EquipmentFragment)
{
	if (AGSEquipItemActor* Item = EquipmentFragment->GetEquippedActor())
	{
		EquippedActors.Remove(Item);
		EquipmentFragment->DestroyEquippedActor();
	}
}
