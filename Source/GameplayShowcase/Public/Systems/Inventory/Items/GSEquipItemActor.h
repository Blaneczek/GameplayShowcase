// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSEquipItemActor.generated.h"

class UGSItemComponent;

UCLASS()
class GAMEPLAYSHOWCASE_API AGSEquipItemActor : public AActor
{
	GENERATED_BODY()

public:
	AGSEquipItemActor();

	void SetEquipMesh(UStaticMesh* Mesh);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> EquipMesh; 
};
