// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GSEquipmentComponent.generated.h"


struct FItemDefinition;
struct FEquipmentFragment;
class AGSPlayerCharacterBase;
class AGSEquipItemActor;
struct FItemInstance;
class UGSInventoryComponent;


/**
 * Manages equipped items for a character.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSEquipmentComponent();

	/** Checks if an equipment slot is currently occupied. */
	FORCEINLINE bool IsSlotEquipped(const FGameplayTag& EquipType) const { return EquippedActors.Contains(EquipType); }

	/** Gets equipped actor for a specific slot. Returns nullptr if slot is empty. */
	FORCEINLINE AGSEquipItemActor* GetEquippedActor(const FGameplayTag& EquipType) const { return EquippedActors.FindRef(EquipType); }

	/** Gets all currently equipped items. */
	FORCEINLINE const TMap<FGameplayTag, AGSEquipItemActor*>& GetAllEquippedActors() const { return EquippedActors; }
	
protected:
	virtual void BeginPlay() override;

private:
	/** Binds inventory component delegates. */
	void BindToInventoryComponent();

	/** 
	 * Attempts to equip an item.
	 * @return	true if successfully equipped
	 */
	bool TryEquipItem(FItemInstance* EquippedItem);

	/** Unequip an item, removing stats and destroying visual actor. */
	void UnequipItem(FItemInstance* UnequippedItem);

	/** 
	 * Checks if item can be equipped. Checks level requirements and slot availability.
	 */
	bool CheckIfCanEquipItem(const FItemDefinition& ItemDefinition) const;

	/** Spawns visual equipment actor attached to character mesh. */
	AGSEquipItemActor* SpawnEquippedActor(FEquipmentFragment* EquipmentFragment);

	/** Removes and destroys equipped actor for a slot. */
	void RemoveEquippedActor(const FGameplayTag& EquipType, FEquipmentFragment* EquipmentFragment);

	TWeakObjectPtr<UGSInventoryComponent> CachedInventoryComponent;
	TWeakObjectPtr<AGSPlayerCharacterBase> CachedOwnerChar;
	TWeakObjectPtr<USkeletalMeshComponent> CachedOwnerSkeletalMesh;

	/** Map of equipped items by equipment type/slot. */
	UPROPERTY()
	TMap<FGameplayTag, AGSEquipItemActor*> EquippedActors;
};
