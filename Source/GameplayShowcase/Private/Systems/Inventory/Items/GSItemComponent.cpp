// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemComponent.h"

#include "GameplayShowcase.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Data/GSItemDataSubsystem.h"
#include "Systems/Inventory/GSInventoryHelper.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"

UGSItemComponent::UGSItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_PlayerChar, ECR_Overlap);

	SetGenerateOverlapEvents(true);
}

UGSItemComponent* UGSItemComponent::FindItemComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSItemComponent>() : nullptr;
}

void UGSItemComponent::MoveItemDefinition(FItemDefinition&& Def)
{
	ItemDefinition = MoveTemp(Def);
	OnItemDefinitionSet.ExecuteIfBound();
}

void UGSItemComponent::LoadDefinition()
{
	if (UGSItemDataSubsystem* ItemDataSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGSItemDataSubsystem>())
	{
		// If data is already loaded, get item
		SetItemDefinition(ItemDataSubsystem);
		
		// If data is not yet loaded, wait for callback
		if (!bDefinitionSet)
		{
			ItemDataSubsystem ->OnItemDataLoadedDelegate.AddUniqueDynamic(this, &UGSItemComponent::SetItemDefinition);
		}		
	}
}

void UGSItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
    OnComponentBeginOverlap.AddDynamic(this, &UGSItemComponent::PickUpZoneEntered);
    OnComponentEndOverlap.AddDynamic(this, &UGSItemComponent::PickUpZoneLeft);
}

void UGSItemComponent::SetItemDefinition(UGSItemDataSubsystem* ItemDataSubsystem)
{
	if (const FItemDefinition* Def = ItemDataSubsystem->GetItemDefinitionByTag(ItemTag))
	{
		ItemDefinition = *Def;
		if (bRandomizeItem)
		{
			RandomizeItem();
		}
		ItemDataSubsystem->OnItemDataLoadedDelegate.RemoveAll(this);
		bDefinitionSet = true;
		OnItemDefinitionSet.ExecuteIfBound();
	}
}

void UGSItemComponent::PickUpZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandlePickUpOverlap(OtherActor, true);
}

void UGSItemComponent::PickUpZoneLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)

{
	HandlePickUpOverlap(OtherActor, false);
}

void UGSItemComponent::HandlePickUpOverlap(AActor* OtherActor, bool bEntered)
{
	if (!OtherActor)
	{
		return;
	}
	
	if (UGSInventoryComponent* InvComponent = UGSInventoryComponent::FindInventoryComponent(OtherActor))
	{
		if (IGSInventoryHelper* InventoryHelper = Cast<IGSInventoryHelper>(InvComponent))
		{
			bEntered ? InventoryHelper->AddItemOnFloor(this) : InventoryHelper->RemoveItemOnFloor(this);
		}
	}
}

void UGSItemComponent::RandomizeItem()
{
	for (auto& Fragment : ItemDefinition.Fragments)
	{
		if (FItemFragment* FragmentPtr = Fragment.GetMutablePtr<FItemFragment>())
		{
			FragmentPtr->Roll();
		}
	}
}






