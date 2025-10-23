// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GSPlayerCharacterBase.generated.h"

class UGSSpringArmComponent;
class UGSCameraComponent;

UCLASS()
class GAMEPLAYSHOWCASE_API AGSPlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGSPlayerCharacterBase();

	UGSSpringArmComponent* GetCameraArm() const;
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UGSCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	TObjectPtr<UGSSpringArmComponent> CameraArm;
};
