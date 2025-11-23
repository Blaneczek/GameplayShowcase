// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GSLootComponent.generated.h"


class AGSWorldItemActor;

USTRUCT(BlueprintType)
struct FLootInfo
{
	GENERATED_BODY()

	FLootInfo() = default;

	UPROPERTY(EditAnywhere)
	FGameplayTag Tag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere)
	float DropChance = 100.f;

	UPROPERTY(EditAnywhere, meta=(ClampMin = 1))
	FInt32Interval QuantityRange = FInt32Interval(0, 0);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSLootComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSLootComponent();

	// Returns the Loot component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category="GS|ItemComponent")
	static UGSLootComponent* FindLootComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void SpawnItems();
	
private:
	void SpawnWorldItem(UWorld* World, const FVector& SpawnLocation, const FGameplayTag& ItemTag);
	
	UPROPERTY(EditAnywhere)
	TArray<FLootInfo> LootTable;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGSWorldItemActor> WorldItemClass;

	UPROPERTY(EditAnywhere)
	float SpawnHeightOffset = 50.f;
};
