// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GSAttackHitboxComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSAttackHitboxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UGSAttackHitboxComponent();

	/** Finds AttackHitboxComponent on an actor. Returns nullptr if not found. */
	static UGSAttackHitboxComponent* FindAttackHitboxComponent(AActor* Actor);
	
protected:
	virtual void BeginPlay() override;

};
