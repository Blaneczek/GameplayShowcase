// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "GSSpringArmComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	UGSSpringArmComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void AddZoomInput(float Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	/** Sets desired zoom range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zoom")
	float MinZoom = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zoom")
	float MaxZoom = 1200.f;

	/** How fast to interpolate the zoom (optional for smooth zoom) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zoom")
	float ZoomInterpSpeed = 8.f;
	
private:
	float TargetZoom;
};
