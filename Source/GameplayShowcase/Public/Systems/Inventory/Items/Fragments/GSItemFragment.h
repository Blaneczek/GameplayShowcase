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
 * 
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

	virtual void LoadData() {};
	virtual void Roll() {};
	
protected:
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
		const FSoftObjectPath Path = Asset.ToSoftObjectPath();
		DataHandle = Manager.RequestAsyncLoad(Path);
	}

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
		const FSoftObjectPath Path = Asset.ToSoftObjectPath();
		DataHandle = Manager.RequestSyncLoad(Path);
	}

	FActiveGameplayEffectHandle ApplyGameplayEffect(IAbilitySystemInterface*, TSoftClassPtr<UGameplayEffect> GameplayEffect,
							const TMap<FGameplayTag, int32>& TagsToMagnitude);
	
	TSharedPtr<FStreamableHandle> DataHandle;
};


USTRUCT(BlueprintType)
struct FStackableFragment : public FItemFragment
{
	GENERATED_BODY()

	/** Gets random number from StackRange when first used and save to DrawnStackNum */
	FORCEINLINE int32 GetStackNum()
	{
		if (!bAlreadyDrawn)
		{
			bAlreadyDrawn = true;
			RolledStackNum = FMath::RandRange(StackRange.Min, StackRange.Max);
		}	
		return RolledStackNum;
	}

	FORCEINLINE void SetNewStackNum(int32 Num) { RolledStackNum = FMath::Max(Num, 1); }
	
private:	
	UPROPERTY(EditAnywhere, meta=(ClampMin="0"))
	FInt32Interval StackRange = FInt32Interval(0, 0);
	
	bool bAlreadyDrawn = false;
	int32 RolledStackNum = 0;
};


USTRUCT()
struct FWidgetFragment : public FItemFragment
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const {};

protected:
	void SetTextFont(UTextBlock* TextBlock) const;
	void AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text, const FLinearColor& TextColor) const;
};


USTRUCT(BlueprintType)
struct FConsumableFragment : public FWidgetFragment
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void LoadData() override;

	void Consume(IAbilitySystemInterface* OwningChar);
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> ConsumeEffect = nullptr;
	
	UPROPERTY(EditAnywhere)
	int32 EffectMagnitude = 0;
	
	UPROPERTY(EditAnywhere)
	FText ConsumableText;
};


USTRUCT(BlueprintType)
struct FItemSize
{
	GENERATED_BODY()

	FItemSize() = default;
	FItemSize(int32 Row, int32 Column)
		:RowSize(Row), ColumnSize(Column)
	{};
	FItemSize(const FVector2D& Size)
		:RowSize(Size.X), ColumnSize(Size.Y)
	{};
	
	UPROPERTY(EditAnywhere)
	int32 RowSize = 0;
	UPROPERTY(EditAnywhere)
	int32 ColumnSize = 0;
};

USTRUCT(BlueprintType)
struct FGridFragment : public FItemFragment
{
	GENERATED_BODY()

	FORCEINLINE const FItemSize& GetGridSize() const { return GridSize; }
	
private:
	UPROPERTY(EditAnywhere)
	FItemSize GridSize;
};


USTRUCT(BlueprintType)
struct FImageFragment : public FItemFragment
{
	GENERATED_BODY()

	FORCEINLINE UTexture2D* GetIcon() const
	{
		return Icon.IsValid() ? Icon.Get() : nullptr;
	}

	virtual void LoadData() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;
};


USTRUCT()
struct FEquipModifier : public FWidgetFragment
{
	GENERATED_BODY()

	virtual void OnEquip(IAbilitySystemInterface* OwningChar) {};
	virtual void OnUnequip(IAbilitySystemInterface* OwningChar);
	
	FORCEINLINE void SetUpgradeLevel(int32 InUpgradeLevel) { CachedUpgradeLevel = InUpgradeLevel; };
	
protected:	
	int32 GetCurveValue(const UCurveTable* CurveTable, const FName& RowName) const;

	FActiveGameplayEffectHandle ActiveGE;
	
private:
	int32 CachedUpgradeLevel = 0;
};


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

USTRUCT(BlueprintType)
struct FAttributeEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bGuaranteed = false;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, meta=(ClampMin=1))
	FInt32Interval MagnitudeRange = FInt32Interval(1, 1);

	bool bAccepted = false;
	int32 RolledValue = 0;
};

USTRUCT(BlueprintType)
struct FAttributeModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(IAbilitySystemInterface* OwningChar) override;
	virtual void LoadData() override;
	virtual void Roll() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> AttributeModifierEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<FAttributeEntry> Attributes;

	int32 AcceptedAttributesNum = 0;
};

USTRUCT(BlueprintType)
struct FEquipmentFragment : public FWidgetFragment
{
	GENERATED_BODY()
	
	void OnEquip(IAbilitySystemInterface* OwningChar);
	void OnUnequip(IAbilitySystemInterface* OwningChar);
	
	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void Roll() override;

	FORCEINLINE int32 GetUpgradeLevel() const { return UpgradeLevel; };
	FORCEINLINE EEquipmentSocket GetEquipmentSocket() const { return SocketAttachPoint; }
	
	AGSEquipItemActor* GetEquippedActor() const;
	AGSEquipItemActor* SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh);
	void DestroyEquippedActor();

	virtual void LoadData() override;
	
private:
	FName GetSocketEnumShortName() const;

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
