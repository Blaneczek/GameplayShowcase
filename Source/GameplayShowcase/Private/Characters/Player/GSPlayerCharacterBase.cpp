// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/Characters/Player/GSPlayerCharacterBase.h"
#include "Player/Camera/GSCameraComponent.h"
#include "Player/Camera/GSSpringArmComponent.h"


AGSPlayerCharacterBase::AGSPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CameraArm = CreateDefaultSubobject<UGSSpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UGSCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm);

}

void AGSPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

UGSSpringArmComponent* AGSPlayerCharacterBase::GetCameraArm() const
{
	return CameraArm;
}

