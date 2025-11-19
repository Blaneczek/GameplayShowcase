// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "StructUtils/InstancedStruct.h"
#include "GSItemFragment.generated.h"

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
	NONE,
	weapon_1H		UMETA(DisplayName="weapon_1H"),
	weapon_2H		UMETA(DisplayName="weapon_2H"),
	weapon_Dagger	UMETA(DisplayName="weapon_Dagger")
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

	FActiveGameplayEffectHandle ApplyGameplayEffect(AGSPlayerCharacterBase* OwningChar, TSoftClassPtr<UGameplayEffect> GameplayEffect,
							const TMap<FGameplayTag, int32>& TagsToMagnitude);
	
	TSharedPtr<FStreamableHandle> DataHandle;
};


USTRUCT(BlueprintType)
struct FStackableFragment : public FItemFragment
{
	GENERATED_BODY()

	/** Gets random number between MinStack and MaxStack (inclusive) when first used and save to DrawnStackNum */
	FORCEINLINE int32 GetStackNum()
	{
		if (!bAlreadyDrawn)
		{
			bAlreadyDrawn = true;
			DrawnStackNum = FMath::RandRange(MinStackNum, MaxStackNum);
		}
		
		return DrawnStackNum;
	}

	FORCEINLINE void SetNewStackNum(int32 Num) { DrawnStackNum = FMath::Max(Num, 1); }
	
private:	
	UPROPERTY(EditAnywhere)
	int32 MinStackNum = 0;
	UPROPERTY(EditAnywhere)
	int32 MaxStackNum = 0;

	bool bAlreadyDrawn = false;
	int32 DrawnStackNum = 0;
};


USTRUCT()
struct FWidgetFragment : public FItemFragment
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const {};

protected:
	void SetTextFont(UTextBlock* TextBlock) const;
	void AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text) const;
};


USTRUCT(BlueprintType)
struct FConsumableFragment : public FWidgetFragment
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void LoadData() override;

	void Consume(AGSPlayerCharacterBase* OwningChar);
	
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

	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) {};
	virtual void OnUnequip(AGSPlayerCharacterBase* OwningChar);
	FORCEINLINE void SetUpgradeLevel(int32 InUpgradeLevel) { UpgradeLevel = InUpgradeLevel; };
	
protected:	
	int32 GetCurveValue(const UCurveTable* CurveTable, const FName& RowName) const;

	FActiveGameplayEffectHandle ActiveGE;
	
private:
	int32 UpgradeLevel = 0;
};


USTRUCT(BlueprintType)
struct FDamageModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) override;
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
	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) override;
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
	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) override;
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
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere)
	FInt32Interval ValueRange;

	int32 RolledValue = 0;
};

USTRUCT(BlueprintType)
struct FAttributeModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) override;
	virtual void LoadData() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UGameplayEffect> AttributeModifierEffect = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<FAttributeEntry> Attributes;
};

USTRUCT(BlueprintType)
struct FEquipmentFragment : public FWidgetFragment
{
	GENERATED_BODY()
	
	void OnEquip(AGSPlayerCharacterBase* OwningChar);
	void OnUnequip(AGSPlayerCharacterBase* OwningChar);
	virtual void AdaptToWidget(UGSItemTooltip* ItemTooltip) const override;
	
	AGSEquipItemActor* GetEquippedActor() const;
	AGSEquipItemActor* SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh);
	void DestroyEquippedActor();

	virtual void LoadData() override;
	
	UPROPERTY(EditAnywhere)
	int32 UpgradeLevel = 0;
	
private:
	FName GetSocketEnumShortName() const;

	bool bEquipped = false;
	
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FEquipModifier>> EquipModifiers;

	UPROPERTY(EditAnywhere)
	EEquipmentSocket SocketAttachPoint = EEquipmentSocket::NONE;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> EquipMesh = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AGSEquipItemActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AGSEquipItemActor> EquippedActor = nullptr;	
};
