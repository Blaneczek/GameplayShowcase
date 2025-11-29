// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSCombatComponent.generated.h"

class UGSAttackHitboxComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartPlayMontageSignature, int32 /*ComboIndex*/)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSCombatComponent();

	/** Finds combat component on an actor. Returns nullptr if not found. */
	static UGSCombatComponent* FindCombatComponent(AActor* Actor);

	FORCEINLINE int32 GetCurrentComboIndex() const { return CurrentComboIndex; }
	
	void BeginComboAttack(int32 LastComboIndex);
	void ResetComboAttack();
	void ActivateHitbox();
	void DeactivateHitbox();
	
	FOnStartPlayMontageSignature OnStartPlayMontage;
	
protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<UGSAttackHitboxComponent> HitboxComponent;
	
	FTimerHandle ComboTimer;	
	int32 CurrentComboIndex = 0;
};
