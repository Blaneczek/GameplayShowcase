// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"

#include "AbilitySystemComponent.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"
#include "Systems/Inventory/Items/GSEquipItemActor.h"
#include "Systems/Inventory/Items/Fragments/GSFragmentTags.h"

void FDamageModifier::AdaptToWidget()
{
	FEquipModifier::AdaptToWidget();
}

void FDamageModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar)
	{
		return;
	}
	
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(OwningChar);
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageModifierEffect, 1.f, ContextHandle);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		
		Spec->SetSetByCallerMagnitude(GSGameplayTags::Attributes::Primary_AttackDamageMin.GetTag(), AttackDamage.Min);
		Spec->SetSetByCallerMagnitude(GSGameplayTags::Attributes::Primary_AttackDamageMax.GetTag(), AttackDamage.Max);
		Spec->SetSetByCallerMagnitude(GSGameplayTags::Attributes::Primary_MagicDamageMin.GetTag(), MagicDamage.Min);
		Spec->SetSetByCallerMagnitude(GSGameplayTags::Attributes::Primary_MagicDamageMax.GetTag(), MagicDamage.Max);
		
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}
void FDamageModifier::OnUnequip(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(GSFragmentTags::Modifiers::Damage.GetTag()));
	}
}

void FEquipmentFragment::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	if (bEquipped)
	{
		return;
	}
	bEquipped = true;
	
	for (auto& Modifier : EquipModifiers)
	{
		FEquipModifier& ModifierRef = Modifier.GetMutable();
		ModifierRef.OnEquip(OwningChar);
	}
}

void FEquipmentFragment::OnUnequip(AGSPlayerCharacterBase* OwningChar)
{
	if (!bEquipped)
	{
		return;
	}
	bEquipped = false;
	
	for (auto& Modifier : EquipModifiers)
	{
		FEquipModifier& ModifierRef = Modifier.GetMutable();
		ModifierRef.OnUnequip(OwningChar);
	}
}

AGSEquipItemActor* FEquipmentFragment::GetEquippedActor() const
{
	return EquippedActor.Get();
}

AGSEquipItemActor* FEquipmentFragment::SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh)
{
	if (!EquipActorClass || !AttachMesh)
	{
		return nullptr;
	}

	AGSEquipItemActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AGSEquipItemActor>(EquipActorClass);
	if (IsValid(SpawnedActor))
	{
		const FName SocketName = GetSocketEnumShortName();
		SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);        
	}
	EquippedActor = SpawnedActor;
	return SpawnedActor;
}

void FEquipmentFragment::DestroyEquippedActor()
{
	EquippedActor.Get()->Destroy();
}

FName FEquipmentFragment::GetSocketEnumShortName() const
{
	const FString EnumString = StaticEnum<EEquipmentSocket>()->GetValueAsString(SocketAttachPoint);
	FString ShortName;
	EnumString.Split(TEXT("::"), nullptr, &ShortName);
	return FName(*ShortName);
}



