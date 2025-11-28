// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/GSItemDefinition.h"
#include "Components/SphereComponent.h"
#include "GSItemComponent.generated.h"

class UGSItemDataSubsystem;
class UGSItemsInfo;

DECLARE_DELEGATE(FOnItemDefinitionSetSignature);

/**
 * Component that represents a physical item in the world that can be picked up.
 * Usage:
 * 1. Attach to an actor representing a world item
 * 2. Set ItemTag to specify which item this represents
 * 3. Bind to OnItemDefinitionSet if needed
 * 3. Call LoadDefinition() to initialize from data subsystem
 * 4. Component automatically handles pickup zone detection
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSItemComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UGSItemComponent();

	/** Finds item component on an actor. Returns nullptr if not found. */
	UFUNCTION(BlueprintPure, Category="GS|ItemComponent")
	static UGSItemComponent* FindItemComponent(const AActor* Actor);

	FORCEINLINE const FItemDefinition& GetItemDefinitionRef() const { return ItemDefinition; }
	FORCEINLINE FItemDefinition& GetItemDefinitionRefMutable() { return ItemDefinition; }

	/** Checks if item definition has been loaded. */
	FORCEINLINE bool HasValidDefinition() const { return ItemDefinition.IsValidDefinition(); }

	/** Sets the item this component represents. Must be called before LoadItemDefinition() if Tag is not set manually in editor. */
	FORCEINLINE void SetItemTag(const FGameplayTag& Tag) { ItemTag = Tag; }
	FORCEINLINE FGameplayTag GetItemTag() const { return ItemTag; }
	
	/** Enables procedural generation (enables Roll() on all fragments). */
	FORCEINLINE void EnableRandomizer() { bRandomizeItem = true; }
	FORCEINLINE bool IsRandomizerEnabled() const { return bRandomizeItem; }

	/** Displays pickup message. Use this when item is picked up. */
	void ItemPickedUp();
	
	/**
	 * Moves an item definition into this component.
	 * Broadcasts OnItemDefinitionSet delegate.
	 */
	void MoveItemDefinition(FItemDefinition&& Def);
	
	/**
	 * Loads item definition from data subsystem based on ItemTag.
	 * Broadcasts OnItemDefinitionSet delegate.
	 */
	void LoadItemDefinition();
	
	FOnItemDefinitionSetSignature OnItemDefinitionSet;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Game item identifier. */
	UPROPERTY(EditAnywhere, Category="GS|ItemComponent")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, Category="GS|ItemComponent")
	FString PickupMessage;
	
private:
	UFUNCTION()
	void OnItemDataLoaded(UGSItemDataSubsystem* ItemDataSubsystem);
	
	UFUNCTION()
    void PickUpZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                           bool bFromSweep, const FHitResult& SweepResult);  
    UFUNCTION()
    void PickUpZoneLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void HandlePickUpZoneOverlap(AActor* OtherActor, bool bEntered);

	void RandomizeItem();
	
	UPROPERTY()
	FItemDefinition ItemDefinition;
	
	bool bRandomizeItem = false;	
};
