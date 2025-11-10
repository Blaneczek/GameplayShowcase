// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "GSItemFragment.generated.h"

class UImage;
class UGameplayEffect;

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
USTRUCT(BlueprintType)
struct FItemFragment
{
	GENERATED_BODY()

	FItemFragment() = default;
	FItemFragment(const FItemFragment&) = default;
	FItemFragment& operator=(const FItemFragment&) = default;
	FItemFragment(FItemFragment&&) = default;
	FItemFragment& operator=(FItemFragment&&) = default;
	virtual ~FItemFragment() = default;

	FORCEINLINE FGameplayTag GetFragmentTag() const { return FragmentTag; }
	FORCEINLINE void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
	
private:
	UPROPERTY(EditAnywhere)
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};



/**********************/
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
			DrawnStackNum = FMath::RandRange(MinStackNum, MAxStackNum);
		}
		
		return DrawnStackNum;
	}

	FORCEINLINE void SetNewStackNum(int32 Num) { DrawnStackNum = FMath::Max(Num, 1); }
	
private:	
	UPROPERTY(EditAnywhere)
	int32 MinStackNum = 0;
	UPROPERTY(EditAnywhere)
	int32 MAxStackNum = 0;

	bool bAlreadyDrawn = false;
	int32 DrawnStackNum = 0;
};
/**********************/


/**********************/
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
/**********************/


/**********************/
USTRUCT(BlueprintType)
struct FImageFragment : public FItemFragment
{
	GENERATED_BODY()

	FORCEINLINE UImage* GetImage() const { return Image; }
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UImage> Image = nullptr;
};
/**********************/


/**********************/
class AGSPlayerCharacterBase;
class AGSEquipItemActor;
USTRUCT(BlueprintType)
struct FEquipModifier : public FItemFragment
{
	GENERATED_BODY()

	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) {};
	virtual void OnUnequip(AGSPlayerCharacterBase* OwningChar) {};
};
/**********************/


/**********************/
USTRUCT(BlueprintType)
struct FDamagePair
{
	GENERATED_BODY()

	FDamagePair() = default;

	UPROPERTY(EditAnywhere)
	int32 Min = 0;
	UPROPERTY(EditAnywhere)
	int32 Max = 0;
};

USTRUCT(BlueprintType)
struct FDamageModifier : public FEquipModifier
{
	GENERATED_BODY()

	virtual void OnEquip(AGSPlayerCharacterBase* OwningChar) override;
	virtual void OnUnequip(AGSPlayerCharacterBase* OwningChar) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageModifierEffect;
	
	UPROPERTY(EditAnywhere)
	FDamagePair AttackDamage;
	UPROPERTY(EditAnywhere)
	FDamagePair MagicDamage;
};
/**********************/


/**********************/
USTRUCT(BlueprintType)
struct FEquipmentFragment : public FItemFragment
{
	GENERATED_BODY()
	
	void OnEquip(AGSPlayerCharacterBase* OwningChar);
	void OnUnequip(AGSPlayerCharacterBase* OwningChar);

	AGSEquipItemActor* GetEquippedActor() const;
	AGSEquipItemActor* SpawnEquipmentActor(USkeletalMeshComponent* AttachMesh);
	void DestroyEquippedActor();

private:
	FName GetSocketEnumShortName() const;

	bool bEquipped = false;
	
	UPROPERTY(EditAnywhere)
	TArray<TInstancedStruct<FEquipModifier>> EquipModifiers;

	UPROPERTY(EditAnywhere)
	EEquipmentSocket SocketAttachPoint = EEquipmentSocket::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGSEquipItemActor> EquipActorClass = nullptr;
	
	TWeakObjectPtr<AGSEquipItemActor> EquippedActor = nullptr;	
};
/**********************/