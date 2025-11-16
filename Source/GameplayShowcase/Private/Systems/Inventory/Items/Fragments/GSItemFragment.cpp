// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"

#include "AbilitySystemComponent.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Components/TextBlock.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"
#include "Systems/Inventory/Items/GSEquipItemActor.h"
#include "Systems/Inventory/Items/Fragments/GSFragmentTags.h"
#include "UI/Widgets/Inventory/GSItemTooltip.h"


void FWidgetFragment::SetTextFont(UTextBlock* TextBlock) const
{
	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.TypefaceFontName = FName("Regular");
	FontInfo.Size = 8;
	TextBlock->SetFont(FontInfo);
	TextBlock->SetJustification(ETextJustify::Center);
}

void FConsumableFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock);
		const FText Text = FText::Format(ConsumableText, EffectMagnitude);
		TextBlock->SetText(Text);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
	}
}

void FConsumableFragment::LoadData()
{
	LoadAsync(ConsumeEffect);
}

void FConsumableFragment::Consume(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar || !ConsumeEffect.IsValid())
	{
		return;
	}
	
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(OwningChar);
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ConsumeEffect.Get(), 1.f, ContextHandle);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		Spec->SetSetByCallerMagnitude(GSFragmentTags::ConsumableFragment.GetTag(), EffectMagnitude);
		
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

void FDamageModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const 
{
	if (UTextBlock* Attack = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(Attack);
		const FString Text = FString::Printf(TEXT("Attack value: %d - %d"), AttackDamage.Min, AttackDamage.Max);
		Attack->SetText(FText::FromString(Text));
		ItemTooltip->AddWidgetToTooltip(Attack);
	}
	if (UTextBlock* MagicAttack = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(MagicAttack);
		const FString Text = FString::Printf(TEXT("Magic Attack value: %d - %d"), MagicDamage.Min, MagicDamage.Max);
		MagicAttack ->SetText(FText::FromString(Text));
		ItemTooltip->AddWidgetToTooltip(MagicAttack);
	}
}

void FEquipmentFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	for (const auto& Modifier : EquipModifiers)
	{
		const FEquipModifier& ModifierRef = Modifier.Get();
		ModifierRef.AdaptToWidget(ItemTooltip);
	}
}

void FImageFragment::LoadData()
{
	LoadSync(Icon);
}

void FEquipmentFragment::LoadData()
{
	LoadAsync(EquipActorClass);
	LoadAsync(EquipMesh);
	for (auto& Modifier : EquipModifiers)
    {
    	FEquipModifier& ModifierRef = Modifier.GetMutable();
    	ModifierRef.LoadData();
    }
}

void FDamageModifier::LoadData()
{
	LoadAsync(DamageModifierEffect);	
}

void FDefenceModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock);
		const FString Text = FString::Printf(TEXT("Defence: %d"), Defence);
		TextBlock->SetText(FText::FromString(Text));
		ItemTooltip->AddWidgetToTooltip(TextBlock);
	}
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock);
		const FString Text = FString::Printf(TEXT("Magic Defence: %d"), MagicDefence);
		TextBlock->SetText(FText::FromString(Text));
		ItemTooltip->AddWidgetToTooltip(TextBlock);
	}
}

void FDefenceModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar || !DefenceModifierEffect.IsValid())
	{
		return;
	}
	
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(OwningChar);
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DefenceModifierEffect.Get(), 1.f, ContextHandle);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		
		Spec->SetSetByCallerMagnitude(GSGameplayTags::Attributes::Primary_Defence.GetTag(), Defence);
		Spec->SetSetByCallerMagnitude(GSGameplayTags::Attributes::Primary_MagicDefence.GetTag(), MagicDefence);
		
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

void FDefenceModifier::OnUnequip(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(GSFragmentTags::Modifiers::Defence.GetTag()));
	}
}

void FDefenceModifier::LoadData()
{
	LoadAsync(DefenceModifierEffect);	
}

void FDamageModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar || !DamageModifierEffect.IsValid())
	{
		return;
	}
	
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(OwningChar);
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageModifierEffect.Get(), 1.f, ContextHandle);
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
	if (AGSEquipItemActor* Actor = EquippedActor.Get())
	{
		return Actor;
	}	
	return nullptr;
}

AGSEquipItemActor* FEquipmentFragment::SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh)
{
	if (!EquipActorClass.IsValid() || !EquipMesh.IsValid() || !AttachMesh)
	{
		return nullptr;
	}

	AGSEquipItemActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AGSEquipItemActor>(EquipActorClass.Get());
	if (IsValid(SpawnedActor))
	{
		const FName SocketName = GetSocketEnumShortName();
		SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		SpawnedActor->SetEquipMesh(EquipMesh.Get());
	}
	EquippedActor = SpawnedActor;
	return SpawnedActor;
}

void FEquipmentFragment::DestroyEquippedActor()
{
	if (AGSEquipItemActor* Actor = EquippedActor.Get())
	{
		Actor->Destroy();
	}	
}

FName FEquipmentFragment::GetSocketEnumShortName() const
{
	const FString EnumString = StaticEnum<EEquipmentSocket>()->GetValueAsString(SocketAttachPoint);
	FString ShortName;
	EnumString.Split(TEXT("::"), nullptr, &ShortName);
	return FName(*ShortName);
}



