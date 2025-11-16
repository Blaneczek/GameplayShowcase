// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Inventory/Items/GSEquipItemActor.h"


AGSEquipItemActor::AGSEquipItemActor()
{
	PrimaryActorTick.bCanEverTick = false;

	EquipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquipMesh"));
	EquipMesh->SetupAttachment(RootComponent);
}

void AGSEquipItemActor::SetEquipMesh(UStaticMesh* Mesh)
{
	EquipMesh->SetStaticMesh(Mesh);
}

void AGSEquipItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}


