// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "StructUtils/InstancedStruct.h"
#include "GSItemFragment.generated.h"

class IAbilitySystemInterface;
class AGSPlayerCharacterBase;
class UTextBlock;
class UGSItemTooltip;
struct FStreamableHandle;
class UImage;
class UGameplayEffect;
class AGSEquipItemActor;

UENUM(BlueprintType)
enum class EEquipmentSocket : uint8
{
	None			UMETA(DisplayName ="None"),
	Weapon_1H		UMETA(DisplayName="One-handed weapon"),
	Weapon_2H		UMETA(DisplayName="Two-handed weapon"),
	Weapon_Dagger	UMETA(DisplayName="Dagger")
};

/**
 * Base fragment for item data composition.
 */
USTRUCT()
struct FItemFragment
{
	GENERATED_BODY()

	FItemFragment() = default;
	FItemFragment(const FItemFragment&) = default;
	FItemFragment& operator=(const FItemFragment&) = default;
	FItemFragment(FItemFragment&&) = default;
	FItemFragment& operator=(FItemFragment&&) = default;
	virtual ~FItemFragment() = default;

	/** Override to load required assets. */
	virtual void LoadData() {};

	/** Override to randomize fragment data */
	virtual void Roll() {};
	
protected:
	/** Loads asset asynchronously using soft references. */
	template <typename AssetType>
	requires TIsDerivedFrom<AssetType, TSoftObjectPtr<typename AssetType::ElementType>>::Value
	|| TIsDerivedFrom<AssetType, TSoftClassPtr<typename AssetType::ElementType>>::Value
	void LoadAsync(const AssetType& Asset)
	{
		if (Asset.IsNull())
		{
			return;
		}
		
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		DataHandle = Manager.RequestAsyncLoad(Asset.ToSoftObjectPath());
	}
	
	/** Loads asset synchronously using soft references. */
	template <typename AssetType>
	requires TIsDerivedFrom<AssetType, TSoftObjectPtr<typename AssetType::ElementType>>::Value
	|| TIsDerivedFrom<AssetType, TSoftClassPtr<typename AssetType::ElementType>>::Value
	void LoadSync(const AssetType& Asset)
	{
		if (Asset.IsNull())
		{
			return;
		}
		
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		DataHandle = Manager.RequestSyncLoad(Asset.ToSoftObjectPath());
	}

	/** Applies a gameplay effect with SetByCaller magnitudes. */
	FActiveGameplayEffectHandle ApplyGameplayEffect(
		IAbilitySystemInterface* Target,
		TSoftClassPtr<UGameplayEffect> GameplayEffect,
		const TMap<FGameplayTag, int32>& TagsToMagnitude) const;
	
	TSharedPtr<FStreamableHandle> DataHandle;
};


/**
 * Fragment for stackable items (consumables, materials, etc.).
 */
USTRUCT(BlueprintType)
struct FStackableFragment : public FItemFragment
{
	GENERATED_BODY()

	virtual void Roll() override;
	
	FORCEINLINE int32 GetStackCount() const { return RolledStackCount; }
	FORCEINLINE void SetStackCount(int32 NewCount) { RolledStackCount = FMath::Max(NewCount, 1); }
	
private:
	UPROPERTY(EditAnywhere, meta=(ClampMin="1"))
	FInt32Interval StackRange = FInt32Interval(1, 1);
	
	int32 RolledStackCount = 0;
};

/**
 * Base fragment for displaying information in tooltip widgets.
 */
USTRUCT()
struct FWidgetFragment : public FItemFragment
{
	GENERATED_BODY()
	
	/** Override to customize tooltip widget display. */
	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const {};

protected:
	/* Default font settings. */
	void SetTextFont(UTextBlock* TextBlock) const;
	
	/** Adds a formatted text block to the tooltip */
	void AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text, const FLinearColor& TextColor) const;
};

/**
 * Fragment for consumable items (potions, food, etc.).
 */
USTRUCT(BlueprintType)
struct FConsumableFragment : public FWidgetFragment
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void LoadData() override;

	/** Applies consumption effect to target character. */
	void Consume(IAbilitySystemInterface* Target);
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> ConsumeEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	int32 EffectMagnitude = 0;
	
	UPROPERTY(EditAnywhere)
	FText ConsumableText;
};

/**
 * Defines item size in grid units (rows x columns) 
 */
USTRUCT(BlueprintType)
struct FItemSize
{
	GENERATED_BODY()

	FItemSize() = default;
	FItemSize(int32 Row, int32 Column) : RowSize(Row), ColumnSize(Column) {}
	FItemSize(const FVector2D& Size) : RowSize(Size.X), ColumnSize(Size.Y) {}
	
	UPROPERTY(EditAnywhere, meta=(ClampMin="1"))
	int32 RowSize = 1;
	UPROPERTY(EditAnywhere, meta=(ClampMin="1"))
	int32 ColumnSize = 1;

	FORCEINLINE bool operator==(const FItemSize& Other) const
	{
		return RowSize == Other.RowSize && ColumnSize == Other.ColumnSize;
	}
};

/**
 * Fragment defining item dimensions in Inventory grid.
 */
USTRUCT(BlueprintType)
struct FGridFragment : public FItemFragment
{
	GENERATED_BODY()

	FORCEINLINE const FItemSize& GetGridSize() const { return GridSize; }
	
private:
	UPROPERTY(EditAnywhere)
	FItemSize GridSize = FItemSize(1, 1);
};

/**
 * Fragment for item icon display.
 */
USTRUCT(BlueprintType)
struct FImageFragment : public FItemFragment
{
	GENERATED_BODY()
	
	virtual void LoadData() override;
	
	FORCEINLINE UTexture2D* GetIcon() const
	{
		return Icon.IsValid() ? Icon.Get() : nullptr;
	}
	
private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
};

/**
 * Base fragment for equipment stat modifiers.
 */
USTRUCT()
struct FEquipModifier : public FWidgetFragment
{
	GENERATED_BODY()

	/** Called when equipment is equipped. */
	virtual void OnEquip(IAbilitySystemInterface* Target) {};

	/** Called when equipment is unequipped. */
	virtual void OnUnequip(IAbilitySystemInterface* Target);
	
	FORCEINLINE void SetUpgradeLevel(int32 InUpgradeLevel) { CachedUpgradeLevel = InUpgradeLevel; };
	
protected:
	/** Returns value from curve table based on upgrade level. */
	int32 GetCurveValue(const UCurveTable* CurveTable, const FName& RowName) const;

	/** Active gameplay effect handle for cleanup effects after unequipping. */
	FActiveGameplayEffectHandle ActiveGE;
	
private:
	int32 CachedUpgradeLevel = 0;
};

/**
 * Modifies damage stats.
 */
USTRUCT(BlueprintType)
struct FDamageModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* OwningChar) override;
	virtual void LoadData() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> DamageModifierEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveTable> DamageCurveTable = nullptr;
};

/**
 * Modifies defence stats.
 */
USTRUCT(BlueprintType)
struct FDefenceModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* OwningChar) override;
	virtual void LoadData() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> DefenceModifierEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveTable> DefenceCurveTable = nullptr;
};

/**
 * Modifies attack speed.
 */
USTRUCT(BlueprintType)
struct FAttackSpeedModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* OwningChar) override;
	virtual void LoadData() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> AttackSpeedModifierEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	int32 BonusAttackSpeedPercent = 0;
};

/**
 * Single attribute modifier entry with random effect magnitude.
 */
USTRUCT(BlueprintType)
struct FAttributeEntry
{
	GENERATED_BODY()

	/**
	 * True - attribute is always added to item
	 * False - 50% chance of being added to the item
	 */
	UPROPERTY(EditAnywhere)
	bool bGuaranteed = false;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	FInt32Interval MagnitudeRange = FInt32Interval(1, 1);

	bool bAccepted = false;

	/** Rolled value within range (set during Roll). */
	int32 RolledValue = 0;
};

/**
 * Modifies attributes.
 */
USTRUCT(BlueprintType)
struct FAttributeModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* OwningChar) override;
	virtual void LoadData() override;
	
	/** Draws attributes and their values that will be added to the item. */
	virtual void Roll() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> AttributeModifierEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<FAttributeEntry> Attributes;

	/** For memory allocation. */
	int32 AcceptedAttributesNum = 0;
};

/**
 * Main fragment managing equipment. 
 */
USTRUCT(BlueprintType)
struct FEquipmentFragment : public FWidgetFragment
{
	GENERATED_BODY()
	
	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
    virtual void Roll() override;
    virtual void LoadData() override;
	
	FORCEINLINE int32 GetUpgradeLevel() const { return UpgradeLevel; };
    FORCEINLINE EEquipmentSocket GetEquipmentSocket() const { return SocketAttachPoint; }
	
	/** Applies all equipment modifiers effects. */
	void OnEquip(IAbilitySystemInterface* Target);
	
	/** Removes all equipment modifiers effects. */
	void OnUnequip(IAbilitySystemInterface* Target);
	
	/** Spawns visual equipment actor attached to character mesh. */
	AGSEquipItemActor* SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh);
	
	/** Destroys spawned equipment actor. */
	void DestroyEquippedActor();	
	
private:
	/** Converts enum value to socket name string */
	FName GetSocketEnumShortName() const;

	/** Sets the item upgrade level based on the DropChance curve. */
	void RollUpgradeLevel();
	
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FEquipModifier>> EquipModifiers;
		
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> UpgradeLevelDropChance;

	UPROPERTY(EditAnywhere)
	EEquipmentSocket SocketAttachPoint = EEquipmentSocket::None;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> EquipMesh = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AGSEquipItemActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AGSEquipItemActor> EquippedActor = nullptr;
	
	int32 UpgradeLevel = 0;
	bool bEquipped = false;
};
