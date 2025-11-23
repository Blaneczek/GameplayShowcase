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


FActiveGameplayEffectHandle FItemFragment::ApplyGameplayEffect(IAbilitySystemInterface* OwningChar,
	TSoftClassPtr<UGameplayEffect> GameplayEffect, const TMap<FGameplayTag, int32>& TagsToMagnitude)
{
	if (!OwningChar || !GameplayEffect.IsValid())
	{
		return FActiveGameplayEffectHandle();
	}
	
	if (UAbilitySystemComponent* ASC = OwningChar->GetAbilitySystemComponent())
	{
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect.Get(), 1.f, ASC->MakeEffectContext());
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


void FWidgetFragment::AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text, const FLinearColor& TextColor) const
{
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock);
		TextBlock->SetText(Text);
		TextBlock->SetColorAndOpacity(TextColor);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
	}
}

void FConsumableFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	const FText Text = FText::Format(ConsumableText, EffectMagnitude);
	AdaptTextBlock(ItemTooltip, Text, FLinearColor(1.f, 1.f, 1.f, 1.f));
}

void FConsumableFragment::LoadData()
{
	LoadAsync(ConsumeEffect);
}

void FConsumableFragment::Consume(IAbilitySystemInterface* OwningChar)
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
	AdaptTextBlock(ItemTooltip, TextAttack, FLinearColor(1.f, 1.f, 1.f, 1.f));

	const int32 MinMagicAttack = GetCurveValue(DamageCurveTable, TEXT("MinMagicAttack"));
	const int32 MaxMagicAttack = GetCurveValue(DamageCurveTable, TEXT("MaxMagicAttack"));
	const FText TextMagicAttack = FText::Format(FText::FromString(TEXT("Magic Attack value: {0} - {1}")), MinMagicAttack, MaxMagicAttack);
	AdaptTextBlock(ItemTooltip, TextMagicAttack, FLinearColor(1.f, 1.f, 1.f, 1.f));
}

void FEquipmentFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	for (const auto& Modifier : EquipModifiers)
	{
		const FEquipModifier& ModifierRef = Modifier.Get();
		ModifierRef.AdaptToWidget(ItemTooltip);
	}
}

void FEquipmentFragment::Roll()
{
	if (!UpgradeLevelDropChance)
	{
		return;
	}

	UpgradeLevel = 0;
	
	const FRichCurve& CurveData = UpgradeLevelDropChance->FloatCurve;
	const float RollValue = FMath::FRandRange(0.f, 100.f);
	UE_LOG(LogTemp, Warning, TEXT("Roll: %f"), RollValue);
	for (int32 i = CurveData.Keys.Num() - 1; i >= 0; --i)
	{
		const float DropChance = CurveData.Keys[i].Value;
		const float Level = CurveData.Keys[i].Time;

		// If drop chance is set to 0 in curve, don't use it 
		if (DropChance <= 0.f)
		{
			continue;
		}
		if (RollValue <= DropChance)
		{
			UpgradeLevel = FMath::Clamp(static_cast<int32>(Level), 0, 9);
			break;
		}
	}

	for (auto& Modifier : EquipModifiers)
	{
		if (FEquipModifier* ModifierRef = Modifier.GetMutablePtr<FEquipModifier>())
		{
			ModifierRef->Roll();
		}	
	}
}

void FImageFragment::LoadData()
{
	LoadSync(Icon);
}

void FEquipModifier::OnUnequip(IAbilitySystemInterface* OwningChar)
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
		return Curve->Eval(CachedUpgradeLevel);
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
	AdaptTextBlock(ItemTooltip, TextDefence, FLinearColor(1.f, 1.f, 1.f, 1.f));

	const int32 MagicDefence = GetCurveValue(DefenceCurveTable, TEXT("MagicDefence"));
	const FText TextMagicDefence = FText::Format(FText::FromString(TEXT("Magic Defence: {0}")), MagicDefence);
	AdaptTextBlock(ItemTooltip, TextMagicDefence, FLinearColor(1.f, 1.f, 1.f, 1.f));
}

void FDefenceModifier::OnEquip(IAbilitySystemInterface* OwningChar)
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
	AdaptTextBlock(ItemTooltip, Text, FLinearColor(1.f, 1.f, 1.f, 1.f));
}

void FAttackSpeedModifier::OnEquip(IAbilitySystemInterface* OwningChar)
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
	for (const auto& Entry : Attributes)
	{
		if (Entry.bAccepted)
		{
			const FText Text = FText::Format(FText::FromString(TEXT("{0}: +{1}"))
            			, UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Entry.AttributeTag)
            			, Entry.RolledValue);
            AdaptTextBlock(ItemTooltip, Text, FLinearColor(0.8f, 0.5f, 0.4f, 1.f));
		}
		
	}
}

void FAttributeModifier::OnEquip(IAbilitySystemInterface* OwningChar)
{
	TMap<FGameplayTag, int32> TagsToMagnitude;
	TagsToMagnitude.Reserve(AcceptedAttributesNum);
	
	for (const auto& Entry : Attributes)
	{
		if (Entry.bAccepted)
		{
			TagsToMagnitude.Add(Entry.AttributeTag, Entry.RolledValue);
		}	
	}
	ActiveGE = ApplyGameplayEffect(OwningChar, AttributeModifierEffect, TagsToMagnitude);
}

void FAttributeModifier::LoadData()
{
	LoadAsync(AttributeModifierEffect);	
}

void FAttributeModifier::Roll()
{
	for (auto& Entry : Attributes)
    {
		Entry.bGuaranteed ? Entry.bAccepted = true : Entry.bAccepted = FMath::RandBool();
		if (Entry.bAccepted)	
		{
			AcceptedAttributesNum++;
			Entry.RolledValue = FMath::RandRange(Entry.MagnitudeRange.Min, Entry.MagnitudeRange.Max);
		}
    }
}

void FDamageModifier::OnEquip(IAbilitySystemInterface* OwningChar)
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

void FEquipmentFragment::OnEquip(IAbilitySystemInterface* OwningChar)
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

void FEquipmentFragment::OnUnequip(IAbilitySystemInterface* OwningChar)
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



