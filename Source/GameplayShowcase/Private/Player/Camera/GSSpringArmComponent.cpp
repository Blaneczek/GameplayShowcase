// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Player/Camera/GSSpringArmComponent.h"


// Sets default values for this component's properties
UGSSpringArmComponent::UGSSpringArmComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TargetZoom = TargetArmLength;
}

void UGSSpringArmComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UGSSpringArmComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FMath::IsNearlyEqual(TargetArmLength, TargetZoom, 5.f))
	{
		TargetArmLength = FMath::FInterpTo(TargetArmLength, TargetZoom, DeltaTime, ZoomInterpSpeed);
	}	
}

void UGSSpringArmComponent::AddZoomInput(float Value)
{
	TargetZoom = FMath::Clamp(TargetZoom - Value, MinZoom, MaxZoom);
}

