// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/GSInventoryComponent.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "GSBlueprintFunctionLibrary.h"
#include "Systems/Inventory/Items/GSItemComponent.h"
#include "Systems/Inventory/Items/GSWorldItemActor.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"


UGSInventoryComponent::UGSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UGSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter = Cast<AGSPlayerCharacterBase>(GetOwner());
}

UGSInventoryComponent* UGSInventoryComponent::FindInventoryComponent(AActor* Actor)
{
	return Actor ? Actor->GetComponentByClass<UGSInventoryComponent>() : nullptr;
}

void UGSInventoryComponent::AddItemOnFloor(UGSItemComponent* ItemComponent)
{
	ItemsOnFloor.Add(ItemComponent);
}

void UGSInventoryComponent::RemoveItemOnFloor(UGSItemComponent* ItemComponent)
{
	ItemsOnFloor.Remove(ItemComponent);
}

bool UGSInventoryComponent::TryEquipItem(FItemInstance* Item)
{
	if (TryEquipItemDelegate.IsBound())
	{
		return TryEquipItemDelegate.Execute(Item);
	}
	return false;
}

bool UGSInventoryComponent::TryEquipItem(const FGuid& ItemID)
{
	if (TryEquipItemDelegate.IsBound())
	{
		return TryEquipItemDelegate.Execute(FindItemInstanceByID(ItemID));
	}
	return false;
}

void UGSInventoryComponent::UnequipItem(const FGuid& ItemID)
{
	if (FItemInstance* Item = FindItemInstanceByID(ItemID))
	{
		UnequipItemDelegate.ExecuteIfBound(Item);
	}	
}

void UGSInventoryComponent::DiscardItemInstance(const FGuid& ItemID)
{
	if (FItemInstance* Item = FindItemInstanceByID(ItemID))
	{
		SpawnWorldItemActor(MoveTemp(Item->GetItemDefinitionMutable()), this, GetOwner()->GetActorLocation());
		RemoveItemInstance(ItemID);
	}
}

void UGSInventoryComponent::TryAddItem()
{
	if (ItemsOnFloor.IsEmpty())
	{
		return;
	}
	
	auto It = ItemsOnFloor.CreateIterator();
	if (UGSItemComponent* ItemComponent = It->Get())
	{
		FItemDefinition& Def = ItemComponent->GetItemDefinitionRefMutable();

		// If item is stackable, try first add it to the existing instance
		if (TryAddNewItemToStack(Def))
		{
			It.RemoveCurrent();
			ItemComponent->GetOwner()->Destroy();
			return;
		}

		// Is not stackable or there is no existing instance or stack for instance has filled up, try to add as new instance
		if (TryAddNewItem(Def))
		{
			It.RemoveCurrent();
			ItemComponent->GetOwner()->Destroy();
		}
	}
}

bool UGSInventoryComponent::TryActivateItemAction(const FGuid& ItemID)
{
	FItemInstance* Instance = FindItemInstanceByID(ItemID);
	if (!Instance)
	{
		return false;
	}

	FItemDefinition& Def = Instance->GetItemDefinitionMutable();
	if (Def.GetFragmentByType<FEquipmentFragment>())
	{
		if (TryEquipItem(Instance))
		{
			OnItemEquippedDelegate.ExecuteIfBound(Def.Type);
			return true;
		}
	}

	if (FConsumableFragment* ConsumeFragment = Def.GetFragmentByTypeMutable<FConsumableFragment>())
	{
		ConsumeFragment->Consume(OwningCharacter.Get());
		HandleStackChange(Instance, -1);		
	}
	return false;
}

void UGSInventoryComponent::TryAddToItemStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom)
{
	if (ItemIDTo == ItemIDFrom)
	{
		return;
	}
	
	FItemInstance* InstanceTo= FindItemInstanceByID(ItemIDTo);
	FItemInstance* InstanceFrom = FindItemInstanceByID(ItemIDFrom);
	if (!InstanceTo || !InstanceFrom)
	{
		return;
	}

	FItemDefinition& DefTo = InstanceTo->GetItemDefinitionMutable();
	FItemDefinition& DefFrom = InstanceFrom->GetItemDefinitionMutable();
	if (DefTo.Name == DefFrom.Name)
	{
		//  Is stackable
		if (DefTo.GetFragmentByTypeMutable<FStackableFragment>())
		{
			const int32 StackLeft = HandleStackChange(InstanceTo, InstanceFrom->GetStackCount());
			if (StackLeft > 0)
			{
				InstanceFrom->SetStackCount(StackLeft);
				OnItemInstanceChangedDelegate.ExecuteIfBound(InstanceFrom);
			}
			else
			{
				RemoveItemInstance(InstanceFrom);
			}		
		}
	}
}

bool UGSInventoryComponent::CheckIfCanAddToStack(const FGuid& ItemIDTo, const FGuid& ItemIDFrom)
{
	if (ItemIDTo == ItemIDFrom)
	{
		return false;
	}
	
	const FItemInstance* InstanceTo = FindItemInstanceByID(ItemIDTo);
	const FItemInstance* InstanceFrom = FindItemInstanceByID(ItemIDFrom);
	if (!InstanceTo || !InstanceFrom)
	{
		return false;
	}

	const FItemDefinition& DefTo = InstanceTo->GetItemDefinition();
	const FItemDefinition& DefFrom = InstanceFrom->GetItemDefinition();
	if (DefTo.Name == DefFrom.Name)
	{
		//  Is stackable
		if (DefTo.GetFragmentByType<FStackableFragment>())
		{
			return InstanceTo->GetStackCount() < MaxStackSize;	
		}
	}
	return false;
}

bool UGSInventoryComponent::TryAddNewItemToStack(FItemDefinition& Def)
{
	if (FStackableFragment* StackableFrag = Def.GetFragmentByTypeMutable<FStackableFragment>())
	{
		FItemInstance* Instance = FindItemInstanceByPredicate([this, Def](const FItemInstance& Instance)
			{
				return Instance.GetItemDefinition().Name.MatchesTagExact(Def.Name)
					&& Instance.GetStackCount() < MaxStackSize;
			});
		
		if (Instance)
		{
			const int32 LeftStackNum = HandleStackChange(Instance, StackableFrag->GetStackNum());		
			if (LeftStackNum > 0)
			{
				// Stack has filled up, decrease Item's StackRange for another TryAddNewItemToStack.
				// If an instance with an incomplete stack is not found, the function will return false.
				StackableFrag->SetNewStackNum(LeftStackNum);
				return TryAddNewItemToStack(Def);
			}		
			return true;
		}
	}
	return false;
}

bool UGSInventoryComponent::TryAddNewItem(FItemDefinition& Def)
{
	// If item doesn't have GridFragment, occupies 1 slot 
	FItemSize ItemSize{1,1};
	if (const FGridFragment* GridFrag = Def.GetFragmentByType<FGridFragment>())
	{
		ItemSize = GridFrag->GetGridSize();
	}
	
	if (UGSInventoryMenuWidgetController* InvController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		FGridInfo GridInfo;
		if (InvController->FindFreeSpace(ItemSize, GridInfo))
		{
			ItemsInstances.Add(CreateItemInstance(Def));
			OnItemInstanceAddedDelegate.ExecuteIfBound(ItemsInstances.Last().GetMutablePtr<FItemInstance>(), GridInfo);
			return true;
		}
	}
	return false;
}

void UGSInventoryComponent::RemoveItemInstance(FItemInstance* Item)
{
	// It's safer to remove by InstanceID
	const FGuid ItemIDToRemove = Item->GetInstanceID();
	const int32 bRemoved = ItemsInstances.RemoveAll([ItemIDToRemove](const TInstancedStruct<FItemInstance>& InstStruct)
	{
		if (const FItemInstance* Instance = InstStruct.GetPtr<FItemInstance>())
		{
			return Instance->GetInstanceID() == ItemIDToRemove;
		}
		return false;
	});
	
	if (bRemoved)
	{
		OnItemInstanceRemovedDelegate.ExecuteIfBound(ItemIDToRemove);
	}	
}

void UGSInventoryComponent::RemoveItemInstance(const FGuid& ItemID)
{
	int32 bRemoved = ItemsInstances.RemoveAll([ItemID](const TInstancedStruct<FItemInstance>& InstStruct)
	{
		if (const FItemInstance* Instance = InstStruct.GetPtr<FItemInstance>())
		{
			return Instance->GetInstanceID() == ItemID;
		}
		return false;
	});
	
	if (bRemoved)
	{
		OnItemInstanceRemovedDelegate.ExecuteIfBound(ItemID);
	}	
}

void UGSInventoryComponent::SpawnWorldItemActor(FItemDefinition&& MovedItemDef, const UObject* WorldContextObject, const FVector& SpawnLocation)
{
	const TSubclassOf<AActor> Class = MovedItemDef.WorldActorClass.LoadSynchronous();
	if (!Class || !WorldContextObject)
	{
		return;
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	if (AGSWorldItemActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AGSWorldItemActor>(Class, SpawnTransform))
	{
		SpawnedActor->SetItemDefinition(MoveTemp(MovedItemDef));
	}
}

int32 UGSInventoryComponent::HandleStackChange(FItemInstance* Instance, int32 ChangeNum)
{
	const int32 Current = Instance->GetStackCount();
	const int32 TargetCount = Current + ChangeNum;
	const int32 ClampedCount = FMath::Clamp(TargetCount, 0, MaxStackSize);
	const int32 NewChangeNum = ClampedCount - Current;
	const int32 LeftChangeNum = ChangeNum - NewChangeNum;
	
	Instance->AddToStack(NewChangeNum);
	OnItemInstanceChangedDelegate.ExecuteIfBound(Instance);

	if (ClampedCount == 0)
	{
		RemoveItemInstance(Instance);
	}
	
	return LeftChangeNum;
}

FItemInstance* UGSInventoryComponent::FindItemInstanceByID(const FGuid& ItemID)
{
	return FindItemInstanceByPredicate([ItemID](const FItemInstance& Instance)
	{
		return Instance.GetInstanceID() == ItemID;
	});
}

FItemInstance* UGSInventoryComponent::FindItemInstanceByNameTag(const FGameplayTag& ItemName)
{
	return FindItemInstanceByPredicate([ItemName](const FItemInstance& Instance)
	{
		return Instance.GetItemDefinition().Name.MatchesTagExact(ItemName);
	});
}

FItemInstance* UGSInventoryComponent::FindItemInstanceByPredicate(TFunctionRef<bool(const FItemInstance&)> Predicate)
{
	for (auto& Instance : ItemsInstances)
	{
		if (FItemInstance* Item = Instance.GetMutablePtr<FItemInstance>())
		{
			if (Predicate(*Item))
			{
				return Item;
			}
		}
	}
	return nullptr;
}

TInstancedStruct<FItemInstance> UGSInventoryComponent::CreateItemInstance(FItemDefinition& Def)
{
	FItemInstance Item;
	Item.CreateID();
	int32 StackNum = 1;
	if (FStackableFragment* StackableFrag = Def.GetFragmentByTypeMutable<FStackableFragment>())
	{
		StackNum = StackableFrag->GetStackNum();
	}
	Item.AddToStack(StackNum);
	Item.MoveItemDefinition(MoveTemp(Def));

	return TInstancedStruct<FItemInstance>::Make(MoveTemp(Item));
}






