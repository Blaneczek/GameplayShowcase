// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSWorldItemActor.h"
#include "GameplayShowcase.h"
#include "GSBlueprintFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "Systems/Inventory/Items/GSItemComponent.h"
#include "Systems/Inventory/Items/Fragments/GSItemFragment.h"


AGSWorldItemActor::AGSWorldItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	SetRootComponent(RootScene);
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionObjectType(ECC_WorldStatic);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_Floor, ECR_Block);
	ItemMesh->BodyInstance.bGenerateWakeEvents = true;
	ItemMesh->SetSimulatePhysics(true);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(ItemMesh);
	
	ItemComponent = CreateDefaultSubobject<UGSItemComponent>(TEXT("ItemComponent"));
	ItemComponent->SetupAttachment(RootComponent);

	bGenerateOverlapEventsDuringLevelStreaming = true;
}

void AGSWorldItemActor::SetItemMesh()
{
	const FItemDefinition& Def = ItemComponent->GetItemDefinitionRef();
	TSoftObjectPtr<UStaticMesh> MeshAsset = Def.WorldItemMesh;
	
	MeshAsset.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([this, MeshAsset](const FSoftObjectPath& Path, UObject* LoadedObj)
	{
		if (UStaticMesh* Mesh = Cast<UStaticMesh>(LoadedObj))
		{
			ItemMesh->SetStaticMesh(Mesh);
		}
	}));
}

void AGSWorldItemActor::MoveItemDefinition(FItemDefinition&& Def)
{
	ItemComponent->MoveItemDefinition(MoveTemp(Def));
}

void AGSWorldItemActor::BeginPlay()
{
	Super::BeginPlay();
	
	ItemMesh->OnComponentSleep.AddDynamic(this, &AGSWorldItemActor::OnItemStoppedFalling);
	ItemComponent->OnItemDefinitionSet.BindUObject(this, &AGSWorldItemActor::OnDefinitionSet);

	if (bLoadDataAutomatically)
	{
		ItemComponent->LoadItemDefinition();
	}
}

void AGSWorldItemActor::OnItemStoppedFalling(UPrimitiveComponent* SleepingComponent, FName BoneName)
{
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetEnableGravity(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGSWorldItemActor::OnDefinitionSet()
{
	SetItemMesh();
	
	const FItemDefinition& Def = ItemComponent->GetItemDefinitionRef();
	const FEquipmentFragment* EquipFragment = Def.FindFragmentByType<FEquipmentFragment>();	
	EquipFragment ?
		ItemNameSet(UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Def.Name), EquipFragment->GetUpgradeLevel()) :
		ItemNameSet(UGSBlueprintFunctionLibrary::GetGameplayTagAsText(Def.Name));
}


