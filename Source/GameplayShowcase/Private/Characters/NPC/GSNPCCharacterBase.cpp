// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/Characters/NPC/GSNPCCharacterBase.h"


// Sets default values
AGSNPCCharacterBase::AGSNPCCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGSNPCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGSNPCCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGSNPCCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

