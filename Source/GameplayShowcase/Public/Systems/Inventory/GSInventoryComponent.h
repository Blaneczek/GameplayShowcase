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

	auto operator<=>(const FGridPosition& Other) const = default;
	
	UPROPERTY(EditAnywhere)
	int32 RowsIndex = 0;
	UPROPERTY(EditAnywhere)
	int32 ColumnsIndex = 0;
};

DECLARE_DELEGATE_RetVal_OneParam(bool, FTryChangeEquipItemStatusSignature, FItemInstance* Instance);
DECLARE_DELEGATE_TwoParams(FOnNewItemAddedSignature, const FItemInstance* Instance, const FGridInfo& GridInfo);
DECLARE_DELEGATE_OneParam(FItemStatusChangedByIDSignature, const FGuid& ItemID);
DECLARE_DELEGATE_OneParam(FItemStatusChangedByInstanceSignature, FItemInstance* Instance);
DECLARE_DELEGATE_OneParam(FOnItemEquippedSignature, const FGameplayTag& EquipType);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSInventoryComponent : public UActorComponent, public IGSInventoryHelper
{
	GENERATED_BODY()

public:
	UGSInventoryComponent();
	static UGSInventoryComponent* FindInventoryComponent(AActor* Actor);

	/* Inventory helper interface */
	virtual void AddItemOnFloor(UGSItemComponent* ItemComponent) override;
	virtual void RemoveItemOnFloor(UGSItemComponent* ItemComponent) override;
	/***/

	FItemInstance* FindItemInstanceByID(const FGuid& ItemID);
	FItemInstance* FindItemInstanceByNameTag(const FGameplayTag& ItemName);
	FItemInstance* FindItemInstanceByPredicate(TFunctionRef<bool(const FItemInstance&)> Predicate);
	
	void TryAddItem();
    bool TryActivateItemAction(const FGuid& ItemID);

	void TryAddToItemStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom);
	bool CheckIfCanAddToStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom);
	
	bool TryEquipItem(FItemInstance* Item); 
	bool TryEquipItem(const FGuid& ItemID); 
	void UnequipItem(const FGuid& ItemID);

	void DiscardItemInstance(const FGuid& ItemID);
	
	FTryChangeEquipItemStatusSignature TryEquipItemDelegate;
	
	FOnNewItemAddedSignature OnItemInstanceAddedDelegate;
	
	FItemStatusChangedByIDSignature OnItemInstanceRemovedDelegate;
	FItemStatusChangedByInstanceSignature OnItemInstanceChangedDelegate;
	FItemStatusChangedByInstanceSignature UnequipItemDelegate;
	
	FOnItemEquippedSignature OnItemEquippedDelegate;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize = 100;
	
private:	
	bool TryAddNewItemToStack(FItemDefinition& Def) ;
	bool TryAddNewItem(FItemDefinition& Def);
	void RemoveItemInstance(FItemInstance* Item);
	void RemoveItemInstance(const FGuid& ItemID);
	void SpawnWorldItemActor(FItemDefinition&& MovedItemDef, const UObject* WorldContextObject, const FVector& SpawnLocation);
	
	int32 HandleStackChange(FItemInstance* Instance, int32 ChangeNum);
	
	TInstancedStruct<FItemInstance> CreateItemInstance(FItemDefinition& Def);
	
	UPROPERTY()
	TArray<TInstancedStruct<FItemInstance>> ItemsInstances;
	
	TWeakObjectPtr<AGSPlayerCharacterBase> OwningCharacter;

	UPROPERTY()
	TSet<TWeakObjectPtr<UGSItemComponent>> ItemsOnFloor;
};
