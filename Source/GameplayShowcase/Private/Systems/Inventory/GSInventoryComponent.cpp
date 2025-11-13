// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/GSInventoryComponent.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "Systems/Inventory/Items/GSItemComponent.h"
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
	return TryEquipItemDelegate.Execute(Item);
}

bool UGSInventoryComponent::TryEquipItem(const FGuid& ItemID)
{
	return TryEquipItemDelegate.Execute(FindItemInstanceByID(ItemID));
}

void UGSInventoryComponent::UnequipItem(const FGuid& ItemID)
{
	if (FItemInstance* Item = FindItemInstanceByID(ItemID))
	{
		UnequipItemDelegate.Execute(Item);
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
		if (TryAddItemToStack(Def))
		{
			It.RemoveCurrent();
			UE_LOG(LogTemp, Warning, TEXT("Item added to inventory: %s"), *ItemComponent->GetOwner()->GetName());
			ItemComponent->GetOwner()->Destroy();
			return;
		}

		// Is not stackable or there is no existing instance or stack for instance has filled up, try to add as new instance
		if (TryAddNewItem(Def))
		{
			It.RemoveCurrent();
			UE_LOG(LogTemp, Warning, TEXT("Item added to inventory: %s"), *ItemComponent->GetOwner()->GetName());
			ItemComponent->GetOwner()->Destroy();
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Can't add item: %s"), *ItemComponent->GetOwner()->GetName());
	}
}

bool UGSInventoryComponent::TryActivateItemAction(const FGuid& ItemID)
{
	FItemInstance* Instance = FindItemInstanceByID(ItemID);
	if (!Instance)
	{
		return false;
	}

	const FItemDefinition& Def = Instance->GetItemDefinition();
	if (Def.GetFragmentByType<FEquipmentFragment>())
	{
		if (TryEquipItem(Instance))
		{
			OnItemEquippedDelegate.Execute(Def.Type);
			return true;
		}
	}

	if (Def.GetFragmentByType<FConsumableFragment>())
	{
		UE_LOG(LogTemp, Warning, TEXT("consume item"));		
	}
	return false;
}

bool UGSInventoryComponent::TryAddItemToStack(FItemDefinition& Def)
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
			int32 StackNum = StackableFrag->GetStackNum();
			const int32 StackSizeLeft = MaxStackSize - Instance->GetStackCount();
			const int32 StackNumToAdd = FMath::Min(StackNum, StackSizeLeft);
			StackNum -= StackNumToAdd;
			Instance->AddToStack(StackNumToAdd);
			OnItemInstanceChangedDelegate.Execute(Instance);
			
			// Stack has filled up, decrease Item's StackNum for another TryAddItem
			if (StackNum > 0)
			{
				StackableFrag->SetNewStackNum(StackNum);
				return false;
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
		if (InvController->TryFindFreeSpace(ItemSize, GridInfo))
		{
			ItemsInstances.Add(CreateItemInstance(Def));
			OnItemInstanceAddedDelegate.Execute(ItemsInstances.Last().Get(), GridInfo);
			return true;
		}
	}
	return false;
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






