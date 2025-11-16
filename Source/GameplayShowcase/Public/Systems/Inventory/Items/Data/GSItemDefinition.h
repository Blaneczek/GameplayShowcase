// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "GSItemDefinition.generated.h"

class UGSItemTooltip;
struct FItemFragment;

/**
 * 
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

	/** Spawns the in-world actor when dropping this item. */
	void SpawnWorldActor(const UObject* WorldContextObject, const FTransform& SpawnTransform);

	/** Finds a mutable fragment of a given type, or nullptr if not found. */
	template<typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	FragmentType* GetFragmentByTypeMutable();

	/** Finds a fragment of a given type, or nullptr if not found. */
	template<typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	const FragmentType* GetFragmentByType() const;

	template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	TArray<FragmentType*> GetAllFragmentsByTypeMutable();

	template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
	TArray<const FragmentType*> GetAllFragmentsByType() const;

	void AdaptItemNameToWidget(UGSItemTooltip* ItemTooltip) const;
	void AdaptItemTypeToWidget(UGSItemTooltip* ItemTooltip) const;
	
	/** Name of this item. */
	UPROPERTY(EditAnywhere)
	FGameplayTag Name;

	/** Type of this item. */
	UPROPERTY(EditAnywhere)
	FGameplayTag Type;

	/** The class of the actor representing this item in the world. */
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> WorldActorClass = nullptr;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> WorldItemMesh = nullptr;
	
	/** Fragments that describe this item’s behavior. */
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FItemFragment>> Fragments;

private:
	/** Empties Fragments array. */
	void ClearFragments();
};

template <typename FragmentType> requires TIsDerivedFrom<FragmentType, FItemFragment>::Value
FragmentType* FItemDefinition::GetFragmentByTypeMutable()
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
const FragmentType* FItemDefinition::GetFragmentByType() const
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
TArray<FragmentType*> FItemDefinition::GetAllFragmentsByTypeMutable()
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
TArray<const FragmentType*> FItemDefinition::GetAllFragmentsByType() const
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

