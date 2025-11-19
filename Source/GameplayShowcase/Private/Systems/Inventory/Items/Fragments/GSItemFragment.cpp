// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"

#include "AbilitySystemComponent.h"
#include "GSBlueprintFunctionLibrary.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Components/TextBlock.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"
#include "Systems/Inventory/Items/GSEquipItemActor.h"
#include "Systems/Inventory/Items/Fragments/GSFragmentTags.h"
#include "UI/Widgets/Inventory/GSItemTooltip.h"


FActiveGameplayEffectHandle FItemFragment::ApplyGameplayEffect(AGSPlayerCharacterBase* OwningChar,
	TSoftClassPtr<UGameplayEffect> GameplayEffect, const TMap<FGameplayTag, int32>& TagsToMagnitude)
{
	if (!OwningChar || !GameplayEffect.IsValid())
	{
		return FActiveGameplayEffectHandle();
	}
	
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(OwningChar);
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect.Get(), 1.f, ContextHandle);
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		for (const auto& Pair : TagsToMagnitude)
		{
			Spec->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
		}	
		return ASC->ApplyGameplayEffectSpecToSelf(*Spec);
	}
	return FActiveGameplayEffectHandle();
}

void FWidgetFragment::SetTextFont(UTextBlock* TextBlock) const
{
	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.TypefaceFontName = FName("Regular");
	FontInfo.Size = 8;
	TextBlock->SetFont(FontInfo);
	TextBlock->SetJustification(ETextJustify::Center);
}


void FWidgetFragment::AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text) const
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock);
		TextBlock->SetText(Text);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
	}
}

void FConsumableFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	const FText Text = FText::Format(ConsumableText, EffectMagnitude);
	AdaptTextBlock(ItemTooltip, Text);
}

void FConsumableFragment::LoadData()
{
	LoadAsync(ConsumeEffect);
}

void FConsumableFragment::Consume(AGSPlayerCharacterBase* OwningChar)
{
	const TMap<FGameplayTag, int32> TagsToMagnitude
	{
		{GSFragmentTags::ConsumableFragment.GetTag(), EffectMagnitude}
	};
	ApplyGameplayEffect(OwningChar, ConsumeEffect, TagsToMagnitude);
}

void FDamageModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	const int32 MinAttack = GetCurveValue(DamageCurveTable, TEXT("MinAttack"));
	const int32 MaxAttack = GetCurveValue(DamageCurveTable, TEXT("MaxAttack"));
	const FText TextAttack = FText::Format(FText::FromString(TEXT("Attack value: {0} - {1}")), MinAttack, MaxAttack);
	AdaptTextBlock(ItemTooltip, TextAttack);

	const int32 MinMagicAttack = GetCurveValue(DamageCurveTable, TEXT("MinMagicAttack"));
	const int32 MaxMagicAttack = GetCurveValue(DamageCurveTable, TEXT("MaxMagicAttack"));
	const FText TextMagicAttack = FText::Format(FText::FromString(TEXT("Magic Attack value: {0} - {1}")), MinMagicAttack, MaxMagicAttack);
	AdaptTextBlock(ItemTooltip, TextMagicAttack);
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

void FEquipModifier::OnUnequip(AGSPlayerCharacterBase* OwningChar)
{
	if (!OwningChar)
	{
		return;
	}
		
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		if (ActiveGE.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ActiveGE);
		}
	}
}

int32 FEquipModifier::GetCurveValue(const UCurveTable* CurveTable, const FName& RowName) const
{
	if (!CurveTable)
	{
		return 0;
	}
	
	static const FString Context = TEXT("Item modifier");
	if (const FRealCurve* Curve = CurveTable->FindCurve(RowName, Context))
	{
		return Curve->Eval(UpgradeLevel);
	}
	return 0;
}

void FEquipmentFragment::LoadData()
{
	LoadAsync(EquipActorClass);
	LoadAsync(EquipMesh);
	for (auto& Modifier : EquipModifiers)
    {
    	FEquipModifier& ModifierRef = Modifier.GetMutable();
    	ModifierRef.LoadData();
		ModifierRef.SetUpgradeLevel(UpgradeLevel);
    }
}

void FDamageModifier::LoadData()
{
	LoadAsync(DamageModifierEffect);	
}

void FDefenceModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	const int32 Defence = GetCurveValue(DefenceCurveTable, TEXT("Defence"));
	const FText TextDefence = FText::Format(FText::FromString(TEXT("Defence: {0}")), Defence);
	AdaptTextBlock(ItemTooltip, TextDefence);

	const int32 MagicDefence = GetCurveValue(DefenceCurveTable, TEXT("MagicDefence"));
	const FText TextMagicDefence = FText::Format(FText::FromString(TEXT("Magic Defence: {0}")), MagicDefence);
	AdaptTextBlock(ItemTooltip, TextMagicDefence);
}

void FDefenceModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	const TMap<FGameplayTag, int32> TagsToMagnitude
	{
		{ GSGameplayTags::Attributes::Primary_Defence.GetTag(), GetCurveValue(DefenceCurveTable, TEXT("Defence")) },
		{ GSGameplayTags::Attributes::Primary_MagicDefence.GetTag(), GetCurveValue(DefenceCurveTable, TEXT("MagicDefence")) },
	};
	ActiveGE = ApplyGameplayEffect(OwningChar, DefenceModifierEffect, TagsToMagnitude);
}

void FDefenceModifier::LoadData()
{
	LoadAsync(DefenceModifierEffect);	
}

void FAttackSpeedModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	const FText Text = FText::Format(FText::FromString(TEXT("Attack speed: +{0}%")), BonusAttackSpeedPercent);
	AdaptTextBlock(ItemTooltip, Text);
}

void FAttackSpeedModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	const TMap<FGameplayTag, int32> TagsToMagnitude
	{
		{GSGameplayTags::Attributes::Primary_AttackSpeed.GetTag(), BonusAttackSpeedPercent},
	};
	ActiveGE = ApplyGameplayEffect(OwningChar, AttackSpeedModifierEffect, TagsToMagnitude);
}

void FAttackSpeedModifier::LoadData()
{
	LoadAsync(AttackSpeedModifierEffect);
}

void FAttributeModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	for (const auto& Attribute : Attributes)
	{
		const FText Text = FText::Format(FText::FromString(TEXT("{0}: +{1}"))
			, UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Attribute.AttributeTag)
			, Attribute.RolledValue);
		AdaptTextBlock(ItemTooltip, Text);
	}
}

void FAttributeModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	TMap<FGameplayTag, int32> TagsToMagnitude;
	for (const auto& Attribute : Attributes)
	{
		TagsToMagnitude.Add(Attribute.AttributeTag, Attribute.RolledValue);
	}
	ActiveGE = ApplyGameplayEffect(OwningChar, AttributeModifierEffect, TagsToMagnitude);
}

void FAttributeModifier::LoadData()
{
	LoadAsync(AttributeModifierEffect);
	
	for (FAttributeEntry& Entry : Attributes)
	{
		Entry.RolledValue = FMath::RandRange(Entry.ValueRange.Min, Entry.ValueRange.Max);
	}
}

void FDamageModifier::OnEquip(AGSPlayerCharacterBase* OwningChar)
{
	const TMap<FGameplayTag, int32> TagsToMagnitude
	{
		{ GSGameplayTags::Attributes::Primary_AttackDamageMin.GetTag(), GetCurveValue(DamageCurveTable, TEXT("MinAttack")) },
		{ GSGameplayTags::Attributes::Primary_AttackDamageMax.GetTag(), GetCurveValue(DamageCurveTable, TEXT("MaxAttack")) },
		{ GSGameplayTags::Attributes::Primary_MagicDamageMin.GetTag(), GetCurveValue(DamageCurveTable, TEXT("MinMagicAttack")) },
		{ GSGameplayTags::Attributes::Primary_MagicDamageMax.GetTag(), GetCurveValue(DamageCurveTable, TEXT("MaxMagicAttack")) },
	};
	ActiveGE = ApplyGameplayEffect(OwningChar, DamageModifierEffect, TagsToMagnitude);
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



