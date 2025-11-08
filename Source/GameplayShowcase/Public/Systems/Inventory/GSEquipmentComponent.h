// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSEquipmentComponent.generated.h"


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
	
	UFUNCTION()
	void OnItemEquipped(FItemInstance& EquippedItem);
	UFUNCTION()
	void OnItemUnequipped(FItemInstance& UnequippedItem);

	AGSEquipItemActor* SpawnEquippedActor(FEquipmentFragment* EquipmentFragment) const;
	void RemoveEquippedActor(FEquipmentFragment* EquipmentFragment);
	
	TWeakObjectPtr<UGSInventoryComponent> InventoryComponent;
	TWeakObjectPtr<AGSPlayerCharacterBase> OwningCharacter;
	TWeakObjectPtr<USkeletalMeshComponent> OwnerSkeletalMesh;

	UPROPERTY()
	TArray<TObjectPtr<AGSEquipItemActor>> EquippedActors;
};
