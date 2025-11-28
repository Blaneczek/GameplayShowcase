// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSCombatComponent();

	/** Finds combat component on an actor. Returns nullptr if not found. */
	static UGSCombatComponent* FindCombatComponent(AActor* Actor);
	
protected:
	virtual void BeginPlay() override;


};
