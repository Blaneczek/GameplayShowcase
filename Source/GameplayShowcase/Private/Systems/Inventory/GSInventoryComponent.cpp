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
	if (!OwningCharacter.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UGSInventoryComponent::BeginPlay | Owner is not AGSPlayerCharacterBase"));
	}
}

UGSInventoryComponent* UGSInventoryComponent::FindInventoryComponent(AActor* Actor)
{
	return Actor ? Actor->GetComponentByClass<UGSInventoryComponent>() : nullptr;
}

void UGSInventoryComponent::AddItemOnFloor(UGSItemComponent* ItemComponent)
{
	if (ItemComponent)
	{
		ItemsOnFloor.Add(ItemComponent);
	}	
}

void UGSInventoryComponent::RemoveItemOnFloor(UGSItemComponent* ItemComponent)
{
	if (ItemComponent)
	{
		ItemsOnFloor.Remove(ItemComponent);
	}	
}

bool UGSInventoryComponent::TryEquipItem(FItemInstance* Item)
{
	if (!Item)
	{
		return false;
	}
	
	if (TryEquipItemDelegate.IsBound())
	{
		return TryEquipItemDelegate.Execute(Item);
	}
	return false;
}

bool UGSInventoryComponent::TryEquipItem(const FGuid& ItemID)
{
	FItemInstance* Item = FindItemInstanceByID(ItemID);
	if (!Item)
	{
		return false;
	}
	
	if (TryEquipItemDelegate.IsBound())
	{
		return TryEquipItemDelegate.Execute(Item);
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

void UGSInventoryComponent::DiscardItemInstance(const FGuid& ItemID, bool bIsEquipped)
{
	if (FItemInstance* Item = FindItemInstanceByID(ItemID))
	{
		if (bIsEquipped)
		{
			UnequipItemDelegate.ExecuteIfBound(Item);
		}
		const AGSPlayerCharacterBase* Owner = OwningCharacter.Get();
		const FVector SpawnLocation = Owner ? Owner->GetActorLocation() : FVector::ZeroVector;
		SpawnWorldItemActor(MoveTemp(Item->GetItemDefinitionMutable()), this, SpawnLocation);
		RemoveItemInstance(ItemID);
	}
}

void UGSInventoryComponent::TryAddItem()
{
	if (ItemsOnFloor.IsEmpty())
	{
		return;
	}

	// Get first item on floor
	auto It = ItemsOnFloor.CreateIterator();
	UGSItemComponent* ItemComponent = It->Get();
	if (!ItemComponent || !ItemComponent->HasValidDefinition())
	{
		It.RemoveCurrent();
		return;
	}

	FItemDefinition& Def = ItemComponent->GetItemDefinitionRefMutable();

	// If item is stackable, try to add to existing stack first
	if (TryAddNewItemToStack(Def))
	{
		It.RemoveCurrent();
		ItemComponent->ItemPickedUp();
		if (AActor* ItemActor = ItemComponent->GetOwner())
		{
			ItemActor->Destroy();
		}
		return;
	}
	
	// If not stackable or no room in existing stacks, add as new item
	if (TryAddNewItem(Def))
	{
		It.RemoveCurrent();
		ItemComponent->ItemPickedUp();
		if (AActor* ItemActor = ItemComponent->GetOwner())
		{
			ItemActor->Destroy();
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

	// Try to equip if item is equipment
	FItemDefinition& Def = Instance->GetItemDefinitionMutable();
	if (Def.FindFragmentByType<FEquipmentFragment>())
	{
		if (TryEquipItem(Instance))
		{
			OnItemEquippedDelegate.ExecuteIfBound(Def.Type);
			return true;
		}
	}

	// Try to consume if item is consumable
	if (FConsumableFragment* ConsumeFragment = Def.FindFragmentByTypeMutable<FConsumableFragment>())
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

	const FItemDefinition& DefTo = InstanceTo->GetItemDefinition();
	const FItemDefinition& DefFrom = InstanceFrom->GetItemDefinition();
	
	// Check if items are same type
	if (!DefTo.Name.MatchesTagExact(DefFrom.Name))
	{
		return;
	}
	
	// Check if stackable
	if (!DefTo.FindFragmentByType<FStackableFragment>())
	{
		return;
	}

	const int32 AmountToTransfer = InstanceFrom->GetStackCount();
	const int32 StackLeft = HandleStackChange(InstanceTo, AmountToTransfer);
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
	if (!DefTo.Name.MatchesTagExact(DefFrom.Name))
	{
		return false;
	}
	
	if (DefTo.FindFragmentByType<FStackableFragment>())
	{
		// Check if target stack has room
		return InstanceTo->GetStackCount() < MaxStackSize;	
	}
	return false;
}

bool UGSInventoryComponent::TryAddNewItemToStack(FItemDefinition& Def)
{
	FStackableFragment* StackableFrag = Def.FindFragmentByTypeMutable<FStackableFragment>();
	if (!StackableFrag)
	{
		return false;
	}
	
	// Find existing instance with same name and available stack space
	FItemInstance* Instance = FindItemInstanceByPredicate([this, Def](const FItemInstance& Instance)
	{
		return Instance.GetItemDefinition().Name.MatchesTagExact(Def.Name)
				&& Instance.GetStackCount() < MaxStackSize;
	});		
	if (!Instance)
	{
		return false;
	}
	
	const int32 AmountToAdd = StackableFrag->GetStackCount();
	const int32 LeftStackNum = HandleStackChange(Instance, AmountToAdd);		
	if (LeftStackNum > 0)
	{
		// Stack filled up, try to add remainder to another stack
		StackableFrag->SetStackCount(LeftStackNum);
		return TryAddNewItemToStack(Def);
	}		
	return true;
}

bool UGSInventoryComponent::TryAddNewItem(FItemDefinition& Def)
{
	// Get item size from grid fragment, or default to 1x1
	FItemSize ItemSize{1,1};
	if (const FGridFragment* GridFrag = Def.FindFragmentByType<FGridFragment>())
	{
		ItemSize = GridFrag->GetGridSize();
	}
	
	UGSInventoryMenuWidgetController* InvController = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this);
	if (!InvController)
	{
		return false;
	}
	
	// Find free space in Inventory grid
	FGridInfo GridInfo;
	if (InvController->FindFreeSpace(ItemSize, GridInfo))
	{
		FItemInstance NewInstance = CreateItemInstance(Def);
		int32 Index = ItemInstances.Add(TInstancedStruct<FItemInstance>::Make(MoveTemp(NewInstance)));
		FItemInstance& StoredInstance = ItemInstances[Index].GetMutable();
		
		// Wait to load all assets before adding to widget
		StoredInstance.OnItemLoaded.BindWeakLambda(this, [this, GridInfo](const FItemInstance* Item)
		{
			OnItemInstanceAddedDelegate.ExecuteIfBound(Item, GridInfo);
		});
		StoredInstance.LoadItemData();		
		return true;
	}
	return false;
}

FItemInstance UGSInventoryComponent::CreateItemInstance(FItemDefinition& Def)
{	
	int32 StackNum = 1;
	if (const FStackableFragment* StackableFrag = Def.FindFragmentByType<FStackableFragment>())
	{
		StackNum = StackableFrag->GetStackCount();
	}
	
	FItemInstance Instance;
	Instance.Initialize(MoveTemp(Def), StackNum);
	return Instance;
}

void UGSInventoryComponent::RemoveItemInstance(FItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	const FGuid ItemIDToRemove = Item->GetInstanceID();
	RemoveItemInstance(ItemIDToRemove);
}

void UGSInventoryComponent::RemoveItemInstance(const FGuid& ItemID)
{
	const int32 RemovedCount = ItemInstances.RemoveAll([&ItemID](const TInstancedStruct<FItemInstance>& InstStruct)
	{
		if (const FItemInstance* Instance = InstStruct.GetPtr<FItemInstance>())
		{
			return Instance->GetInstanceID() == ItemID;
		}
		return false;
	});
	
	if (RemovedCount > 0)
	{
		OnItemInstanceRemovedDelegate.ExecuteIfBound(ItemID);
	}
}

void UGSInventoryComponent::SpawnWorldItemActor(FItemDefinition&& MovedItemDef, const UObject* WorldContextObject, const FVector& SpawnLocation)
{
    if (!WorldContextObject)
    {
	    return;
    }

    TSoftClassPtr<AActor> ActorClassPtr = MovedItemDef.WorldActorClass;
    if (UClass* LoadedClass = ActorClassPtr.Get())
    {
    	SpawnActor(MoveTemp(MovedItemDef), LoadedClass, WorldContextObject, SpawnLocation);
        return;
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    TWeakObjectPtr<UGSInventoryComponent> WeakThis(this);
    TWeakObjectPtr<const UObject> WeakWorldContext(WorldContextObject);
	
    Streamable.RequestAsyncLoad(ActorClassPtr.ToSoftObjectPath(), FStreamableDelegate::CreateLambda(
    	[WeakThis, WeakWorldContext, ActorClassPtr, SpawnLocation, CapturedItemDef = MoveTemp(MovedItemDef)]() mutable
        {
			if (!WeakThis.IsValid() || !WeakWorldContext.IsValid())
            {
                return;
            }

            if (UClass* LoadedClass = ActorClassPtr.Get())
            {
                WeakThis->SpawnActor(MoveTemp(CapturedItemDef), LoadedClass, WeakWorldContext.Get(), SpawnLocation);
            }
		})
	);
}

void UGSInventoryComponent::SpawnActor(FItemDefinition&& ItemDef, UClass* ClassToSpawn, const UObject* WorldContext,
	const FVector& Location)
{
	if (!ClassToSpawn || !WorldContext)
	{
		return;
	}
	UWorld* CachedWorld = WorldContext->GetWorld();
	if (!CachedWorld)
	{
		return;
	}

	FTransform SpawnTransform;
	const FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0.0f, 50.f);
	SpawnTransform.SetLocation(Location + FVector(RandomOffset.X, RandomOffset.Y, 0));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (AGSWorldItemActor* SpawnedActor = CachedWorld->SpawnActor<AGSWorldItemActor>(ClassToSpawn, SpawnTransform, SpawnParams))
	{
		SpawnedActor->MoveItemDefinition(MoveTemp(ItemDef));
	}
}

int32 UGSInventoryComponent::HandleStackChange(FItemInstance* Instance, int32 ChangeNum)
{
	if (!Instance)
	{
		return ChangeNum;
	}
	
	const int32 CurrentCount = Instance->GetStackCount();
	const int32 TargetCount = CurrentCount + ChangeNum;
	const int32 ClampedCount = FMath::Clamp(TargetCount, 0, MaxStackSize);
	const int32 ActualChange = ClampedCount - CurrentCount;
	const int32 RemainingChange = ChangeNum - ActualChange;
	
	Instance->AddToStack(ActualChange);
	OnItemInstanceChangedDelegate.ExecuteIfBound(Instance);

	// Remove item if stack is empty
	if (ClampedCount == 0)
	{
		RemoveItemInstance(Instance);
	}
	
	return RemainingChange;
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
	for (auto& Instance : ItemInstances)
	{
		if (FItemInstance* InstancePtr = Instance.GetMutablePtr<FItemInstance>())
		{
			if (Predicate(*InstancePtr))
			{
				return InstancePtr;
			}
		}
	}
	return nullptr;
}







