// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSItemComponent.h"

#include "GameplayShowcase.h"
#include "GSBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Inventory/Items/Data/GSItemDataSubsystem.h"
#include "Systems/Inventory/GSInventoryHelper.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"
#include "UI/Controllers/GSOverlayWidgetController.h"

UGSItemComponent::UGSItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_PlayerChar, ECR_Overlap);
	SetGenerateOverlapEvents(true);
	InitSphereRadius(100.0f);
}

UGSItemComponent* UGSItemComponent::FindItemComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSItemComponent>() : nullptr;
}

void UGSItemComponent::BeginPlay()
{
	Super::BeginPlay();
	
    OnComponentBeginOverlap.AddDynamic(this, &UGSItemComponent::PickUpZoneEntered);
    OnComponentEndOverlap.AddDynamic(this, &UGSItemComponent::PickUpZoneLeft);
}

void UGSItemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGSItemDataSubsystem* ItemDataSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGSItemDataSubsystem>())
	{
		ItemDataSubsystem->OnItemDataLoaded.RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UGSItemComponent::MoveItemDefinition(FItemDefinition&& Def)
{
	if (!Def.IsValidDefinition())
	{
		return;
	}
	
	ItemDefinition = MoveTemp(Def);
	OnItemDefinitionSet.ExecuteIfBound();
}

void UGSItemComponent::LoadItemDefinition()
{
	if (!ItemTag.IsValid())
	{
		return;
	}
	
	if (UGSItemDataSubsystem* ItemDataSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGSItemDataSubsystem>())
	{
		if (ItemDataSubsystem->IsDataLoaded())
		{
			// If data is already loaded, get item
			OnItemDataLoaded(ItemDataSubsystem);
		}
		else
		{
			// Wait for data to load
			ItemDataSubsystem ->OnItemDataLoaded.AddUObject(this, &UGSItemComponent::OnItemDataLoaded);
		}	
	}
}

void UGSItemComponent::OnItemDataLoaded(UGSItemDataSubsystem* ItemDataSubsystem)
{
	if (!ItemDataSubsystem)
	{
		return;
	}
	
	if (const FItemDefinition* Def = ItemDataSubsystem->FindItemDefinition(ItemTag))
	{
		ItemDefinition = *Def;
		if (bRandomizeItem)
		{
			RandomizeItem();
		}
		OnItemDefinitionSet.ExecuteIfBound();
		
		ItemDataSubsystem->OnItemDataLoaded.RemoveAll(this);	
	}
}

void UGSItemComponent::RandomizeItem()
{
	if (!ItemDefinition.IsValidDefinition())
	{
		return;
	}
	
	for (auto& Fragment : ItemDefinition.Fragments)
	{
		if (FItemFragment* FragmentPtr = Fragment.GetMutablePtr<FItemFragment>())
		{
			FragmentPtr->Roll();
		}
	}
}

void UGSItemComponent::PickUpZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandlePickUpZoneOverlap(OtherActor, true);
}

void UGSItemComponent::PickUpZoneLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)

{
	HandlePickUpZoneOverlap(OtherActor, false);
}

void UGSItemComponent::HandlePickUpZoneOverlap(AActor* OtherActor, bool bEntered)
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

void UGSItemComponent::ItemPickedUp()
{
	if (!ItemDefinition.IsValidDefinition())
	{
		return;
	}
	
	if (UGSOverlayWidgetController* OverlayController = UGSBlueprintFunctionLibrary::GetOverlayWidgetController(this))
	{
		OverlayController->OnItemPickUpMessage(UGSBlueprintFunctionLibrary::GetGameplayTagAsText(ItemDefinition.Name));
	}		
}





