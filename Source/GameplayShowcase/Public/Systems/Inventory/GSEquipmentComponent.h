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


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSEquipmentComponent();

protected:
	virtual void BeginPlay() override;

private:
	void InitInventoryComponent();
	
	bool TryEquipItem(FItemInstance* EquippedItem);
	void UnequipItem(FItemInstance* UnequippedItem);
	bool CheckIfCanEquipItem(const FItemDefinition& ItemDefinition);
	
	AGSEquipItemActor* SpawnEquippedActor(FEquipmentFragment* EquipmentFragment) const;
	void RemoveEquippedActor(const FGameplayTag& EquipType, FEquipmentFragment* EquipmentFragment);
	
	TWeakObjectPtr<UGSInventoryComponent> InventoryComponent;
	TWeakObjectPtr<AGSPlayerCharacterBase> OwningCharacter;
	TWeakObjectPtr<USkeletalMeshComponent> OwnerSkeletalMesh;

	UPROPERTY()
	TMap<FGameplayTag, AGSEquipItemActor*> EquippedActors;
};
