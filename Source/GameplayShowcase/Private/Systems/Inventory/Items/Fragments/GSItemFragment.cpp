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


// ============================================================================
// FItemFragment
// ============================================================================

FActiveGameplayEffectHandle FItemFragment::ApplyGameplayEffect(IAbilitySystemInterface* Target,
	TSoftClassPtr<UGameplayEffect> GameplayEffect, const TMap<FGameplayTag, int32>& TagsToMagnitude) const
{
	if (!Target || !GameplayEffect.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FItemFragment::ApplyGameplayEffect | Invalid target or effect."));
		return FActiveGameplayEffectHandle();
	}
	
	UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("FItemFragment::ApplyGameplayEffect | Target has no ASC."));
		return FActiveGameplayEffectHandle();
	}

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffect.Get(), 1.f, ASC->MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	for (const auto& Pair : TagsToMagnitude)
	{
		Spec->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
	}	
	return ASC->ApplyGameplayEffectSpecToSelf(*Spec);
}



// ============================================================================
// FWidgetFragment
// ============================================================================

void FWidgetFragment::SetTextFont(UTextBlock* TextBlock, const FLinearColor& TextColor) const
{
	if (!TextBlock)
	{
		return;
	}
	
	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.TypefaceFontName = FName("Regular");
	FontInfo.Size = 8;
	TextBlock->SetFont(FontInfo);
	TextBlock->SetJustification(ETextJustify::Center);
	TextBlock->SetColorAndOpacity(TextColor);
}

void FWidgetFragment::AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text, const FLinearColor& TextColor) const
{
	if (!ItemTooltip)
	{
		return;
	}
	
	if (UTextBlock* TextBlock = NewObject<UTextBlock>(ItemTooltip))
	{
		SetTextFont(TextBlock, TextColor);
		TextBlock->SetText(Text);
		ItemTooltip->AddWidgetToTooltip(TextBlock);
	}
}

// ============================================================================
// FStackableFragment
// ============================================================================

void FStackableFragment::Roll()
{
	RolledStackCount = FMath::RandRange(StackRange.Min, StackRange.Max);
}

// ============================================================================
// FConsumableFragment
// ============================================================================

void FConsumableFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	const FText Text = FText::Format(ConsumableText, EffectMagnitude);
	AdaptTextBlock(ItemTooltip, Text, FLinearColor(1.f, 1.f, 1.f, 1.f));
}

void FConsumableFragment::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(ConsumeEffect.ToSoftObjectPath());
}

void FConsumableFragment::Consume(IAbilitySystemInterface* Target)
{
	const TMap<FGameplayTag, int32> TagsToMagnitude
	{
		{ GSFragmentTags::ConsumableFragment.GetTag(), EffectMagnitude }
	};
	ApplyGameplayEffect(Target, ConsumeEffect, TagsToMagnitude);
}

// ============================================================================
// FImageFragment
// ============================================================================

void FImageFragment::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(Icon.ToSoftObjectPath());
}

// ============================================================================
// FEquipModifier
// ============================================================================

void FEquipModifier::OnUnequip(IAbilitySystemInterface* Target)
{
	if (!Target)
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent();
	if (ASC && ActiveGE.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveGE);
	}
}

int32 FEquipModifier::GetCurveValue(const UCurveTable* CurveTable, const FName& RowName) const
{
	if (!CurveTable)
	{
		return 0;
	}
	
	static const FString ContextString = TEXT("Item modifier");
	const FRealCurve* Curve = CurveTable->FindCurve(RowName, ContextString);
	
	return Curve ? Curve->Eval(CachedUpgradeLevel) : 0;
}

// ============================================================================
// FDamageModifier
// ============================================================================

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

void FDamageModifier::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(DamageModifierEffect.ToSoftObjectPath());
}

// ============================================================================
// FDefenceModifier
// ============================================================================

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

void FDefenceModifier::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(DefenceModifierEffect.ToSoftObjectPath());
}

// ============================================================================
// FAttackSpeedModifier
// ============================================================================

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

void FAttackSpeedModifier::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(AttackSpeedModifierEffect.ToSoftObjectPath());
}


// ============================================================================
// FAttributeModifier
// ============================================================================

void FAttributeModifier::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	for (const auto& Entry : Attributes)
	{
		if (Entry.bAccepted)
		{
			const FText Text = FText::Format(
				FText::FromString(TEXT("{0}: +{1}"))
				, UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Entry.AttributeTag)
				, Entry.RolledValue
			);	
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

void FAttributeModifier::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(AttributeModifierEffect.ToSoftObjectPath());
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

// ============================================================================
// FEquipmentFragment
// ============================================================================

void FEquipmentFragment::AdaptToWidget(UGSItemTooltip* ItemTooltip) const
{
	for (const auto& Modifier : EquipModifiers)
	{
		if (const FEquipModifier* ModifierRef = Modifier.GetPtr())
		{
			ModifierRef->AdaptToWidget(ItemTooltip);
		}	
	}
}

void FEquipmentFragment::Roll()
{
	RollUpgradeLevel();

	for (auto& Modifier : EquipModifiers)
	{
		if (FEquipModifier* ModifierRef = Modifier.GetMutablePtr<FEquipModifier>())
		{
			ModifierRef->Roll();
		}	
	}
}

void FEquipmentFragment::AddSoftObjectPath(TArray<FSoftObjectPath>& Paths)
{
	Paths.Add(EquipActorClass.ToSoftObjectPath());
	Paths.Add(EquipMesh.ToSoftObjectPath());
	
	for (auto& Modifier : EquipModifiers)
	{
		if (FEquipModifier* ModifierRef = Modifier.GetMutablePtr())
		{
			ModifierRef->AddSoftObjectPath(Paths);
			ModifierRef->SetUpgradeLevel(UpgradeLevel);
		}	
	}
}

void FEquipmentFragment::OnEquip(IAbilitySystemInterface* Target)
{
	if (bEquipped)
	{
		UE_LOG(LogTemp, Warning, TEXT("FEquipmentFragment::OnEquip | Already equipped."));
		return;
	}
	
	bEquipped = true;	
	for (auto& Modifier : EquipModifiers)
	{
		if (FEquipModifier* ModifierPtr = Modifier.GetMutablePtr())
        {
        	ModifierPtr->OnEquip(Target);
        }
	}
}

void FEquipmentFragment::OnUnequip(IAbilitySystemInterface* Target)
{
	if (!bEquipped)
	{
		return;
	}
	
	bEquipped = false;	
	for (auto& Modifier : EquipModifiers)
	{
		if (FEquipModifier* ModifierPtr = Modifier.GetMutablePtr())
		{
			ModifierPtr->OnUnequip(Target);
		}
	}
}

AGSEquipItemActor* FEquipmentFragment::SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh)
{
	if (!EquipActorClass.IsValid() || !EquipMesh.IsValid() || !AttachMesh)
	{
		return nullptr;
	}
	
	UWorld* CachedWorld = AttachMesh->GetWorld();
	if (!CachedWorld)
	{
		return nullptr;
	}
	
	AGSEquipItemActor* SpawnedActor = CachedWorld->SpawnActor<AGSEquipItemActor>(EquipActorClass.Get());
	if (!IsValid(SpawnedActor))
	{
		UE_LOG(LogTemp, Error, TEXT("FEquipmentFragment::SpawnEquipmentActor | Failed to spawn actor"));
		return nullptr;
	}

	const FName SocketName = GetSocketEnumShortName();
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	SpawnedActor->SetEquipMesh(EquipMesh.Get());
	EquippedActor = SpawnedActor;
	
	return SpawnedActor;
}

void FEquipmentFragment::DestroyEquippedActor()
{
	if (AGSEquipItemActor* Actor = EquippedActor.Get())
	{
		Actor->Destroy();
		EquippedActor.Reset();
	}	
}

FName FEquipmentFragment::GetSocketEnumShortName() const
{
	const FString EnumString = StaticEnum<EEquipmentSocket>()->GetValueAsString(SocketAttachPoint);
	FString ShortName;
	if (EnumString.Split(TEXT("::"), nullptr, &ShortName))
	{
		return FName(*ShortName);
	}
	return NAME_None;
}

void FEquipmentFragment::RollUpgradeLevel()
{
	if (!UpgradeLevelDropChance)
	{
		return;
	}

	UpgradeLevel = 0;
	
	const FRichCurve& CurveData = UpgradeLevelDropChance->FloatCurve;
	if (CurveData.Keys.Num() == 0)
	{
		return;
	}
	
	const float RollValue = FMath::FRandRange(0.f, 100.f);
	
	// Iterate from highest to lowest level
	for (int32 i = CurveData.Keys.Num() - 1; i >= 0; --i)
	{
		const FRichCurveKey& Key = CurveData.Keys[i];
		const float DropChance = Key.Value;
		const int32 Level = static_cast<int32>(Key.Time);

		// Skip levels with 0% drop chance
		if (DropChance <= 0.f)
		{
			continue;
		}
		if (RollValue <= DropChance)
		{
			UpgradeLevel = FMath::Clamp(Level, 0, 9);
			return;
		}
	}
}



