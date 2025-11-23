// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "GSItemDefinition.generated.h"

class UTextBlock;
class UGSItemTooltip;
struct FItemFragment;

/**
 * Defines an item's data and behavior through a composition of fragments.
 * Items are represented as lightweight data structures.
 */
USTRUCT(BlueprintType)
struct FItemDefinition
{
	GENERATED_BODY()
	
	FItemDefinition() = default;
	FItemDefinition(const FItemDefinition&) = default;
	FItemDefinition& operator=(const FItemDefinition&) = default;
	FItemDefinition(FItemDefinition&&) noexcept = default;
	FItemDefinition& operator=(FItemDefinition&&) noexcept = default;

	/** Finds the first mutable fragment of the specified type, or nullptr if not found. */
	template<typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	FragmentType* FindFragmentByTypeMutable();

	/** Finds the first fragment of the specified type, or nullptr if not found. */
	template<typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	const FragmentType* FindFragmentByType() const;

	/** Retrieves all mutable fragments of the specified type. */
	template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	TArray<FragmentType*> FindAllFragmentsByTypeMutable();
	
	/** Retrieves all fragments of the specified type. */
	template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	TArray<const FragmentType*> FindAllFragmentsByType() const;
	
	/** Adds item name text to tooltip with upgrade level if exists. */
	void AdaptItemNameToWidget(UGSItemTooltip* ItemTooltip) const;
	
	/** Adds item type text to tooltip. */
	void AdaptItemTypeToWidget(UGSItemTooltip* ItemTooltip) const;
	TPair<UTextBlock*, int32> AdaptItemLevelToWidget(UGSItemTooltip* ItemTooltip) const;
	
	/** Display name of this item (via gameplay tag). */
	UPROPERTY(EditAnywhere)
	FGameplayTag Name;

	/** Type of this item (via gameplay tag). */
	UPROPERTY(EditAnywhere)
	FGameplayTag Type;

	/** Required character level to use this item. */
	UPROPERTY(EditAnywhere)
	int32 Level = 0;

	/** Actor class spawned when this item is dropped in the world. */
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> WorldActorClass = nullptr;

	/** Mesh displayed when this item is dropped in the world. */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> WorldItemMesh = nullptr;

	UPROPERTY(EditAnywhere)
	FLinearColor ItemNameColor = FLinearColor(1.f, 0.7f, 0.f,1.f);
	UPROPERTY(EditAnywhere)
	FLinearColor PermittedItemLevelColor = FLinearColor::Green;
	UPROPERTY(EditAnywhere)
	FLinearColor ForbiddenItemLevelColor = FLinearColor::Red;
	
	/** Fragments that compose this item's behavior and properties. */
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FItemFragment>> Fragments;

private:
	/** Empties Fragments array. */
	void ClearFragments();

	void SetTextFont(UTextBlock* TextBlock, const FLinearColor& TextColor) const;
	UTextBlock* AdaptTextBlock(UGSItemTooltip* ItemTooltip, const FText& Text, const FLinearColor& TextColor) const;
};

template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
FragmentType* FItemDefinition::FindFragmentByTypeMutable()
{
	for (TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (FragmentType* FragmentPtr = Fragment.GetMutablePtr<FragmentType>())
		{
			return FragmentPtr;
		}
	}	
	return nullptr;
}

template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
const FragmentType* FItemDefinition::FindFragmentByType() const
{
	for (const TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (const FragmentType* FragmentPtr = Fragment.GetPtr<FragmentType>())
		{
			return FragmentPtr;
		}
	}	
	return nullptr;
}

template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
TArray<FragmentType*> FItemDefinition::FindAllFragmentsByTypeMutable()
{
	TArray<FragmentType*> OutFragments;
	OutFragments.Reserve(Fragments.Num());
	for (TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (FragmentType* FragmentPtr = Fragment.GetMutablePtr<FragmentType>())
		{
			OutFragments.Add(FragmentPtr);
		}
	}	
	return OutFragments;
}

template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
TArray<const FragmentType*> FItemDefinition::FindAllFragmentsByType() const
{
	TArray<const FragmentType*> OutFragments;
	OutFragments.Reserve(Fragments.Num());
	for (const TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{		
		if (const FragmentType* FragmentPtr = Fragment.GetPtr<FragmentType>())
		{
			OutFragments.Add(FragmentPtr);
		}
	}	
	return OutFragments;
}

