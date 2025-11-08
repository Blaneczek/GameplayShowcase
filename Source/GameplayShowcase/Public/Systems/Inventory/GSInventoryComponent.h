// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GSInventoryHelper.h"
#include "Components/ActorComponent.h"
#include "Items/GSItemInstance.h"
#include "StructUtils/InstancedStruct.h"
#include "GSInventoryComponent.generated.h"

struct FGridInfo;
class AGSPlayerCharacterBase;
class UGSWidgetBase;


USTRUCT(BlueprintType)
struct FGridPosition
{
	GENERATED_BODY()

	FGridPosition() = default;
	FGridPosition(int32 Rows, int32 Columns)
		: RowsIndex(Rows), ColumnsIndex(Columns) {}

	UPROPERTY(EditAnywhere)
	int32 RowsIndex = 0;
	UPROPERTY(EditAnywhere)
	int32 ColumnsIndex = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChangedSignature, FItemInstance&, Item);
DECLARE_MULTICAST_DELEGATE_OneParam(FItemInstanceStatusChangedSiganture, const FItemInstance& Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FNewItemSignature, const FItemDefinition& ItemDef, const FGridInfo& GridInfo);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSInventoryComponent : public UActorComponent, public IGSInventoryHelper
{
	GENERATED_BODY()

public:
	UGSInventoryComponent();
	static UGSInventoryComponent* FindInventoryComponent(AActor* Actor);

	/* Inventory interface */
	virtual void AddItemOnFloor(UGSItemComponent* ItemComponent) override;
	virtual void RemoveItemOnFloor(UGSItemComponent* ItemComponent) override;
	/***/
	
	UFUNCTION(BlueprintCallable)
	void EquipItem(FItemInstance& Item);
	UFUNCTION(BlueprintCallable)
	void UnequipItem(FItemInstance& Item);

	void TryAddItem();
	
	FItemEquipStatusChangedSignature OnItemEquipped;
	FItemEquipStatusChangedSignature OnItemUnequipped;

	FNewItemSignature OnNewItemAdded;
	FItemInstanceStatusChangedSiganture OnInstanceChanged;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize = 100;
	
private:	
	bool TryAddItemToStack(FItemDefinition& Def) ;
	bool TryAddNewItem(FItemDefinition& Def);

	FItemInstance* FindItemInstance(const FGameplayTag& TagName);
	
	TInstancedStruct<FItemInstance> CreateItemInstance(FItemDefinition& Def);
	
	UPROPERTY(EditAnywhere)
	TArray<TInstancedStruct<FItemInstance>> ItemsInstances;
	
	TWeakObjectPtr<AGSPlayerCharacterBase> OwningCharacter;

	UPROPERTY()
	TSet<TWeakObjectPtr<UGSItemComponent>> ItemsOnFloor;
};
