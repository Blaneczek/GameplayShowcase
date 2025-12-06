// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSCombatComponent.generated.h"

class AGSPlayerCharacterBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSCombatComponent();

	/** Finds combat component on an actor. Returns nullptr if not found. */
	static UGSCombatComponent* FindCombatComponent(AActor* Actor);

	FORCEINLINE bool IsAttacking () const { return bAttacking ; }
	FORCEINLINE void SetIsAttacking (bool IsAttacking) { bAttacking = IsAttacking; }
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void StartComboAttackTrace(float AttackRange, const FCollisionShape& TraceShape);
	void StopComboAttackTrace();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bDebugTrace = false;
	
private:
	void DoComboAttackTrace();
	void SweepShape(const FVector& From, const FVector& To, bool bGap = false);
	void DrawDebugShape(const FVector& Start, const FVector& End, bool bHit, const TArray<FHitResult>& OutHits, bool bGap = false);

	void FillSweepGaps(int32 Times, const FVector& LastStart, const FVector& Start, const FVector& LastEnd, const FVector& End);
	
	TWeakObjectPtr<AGSPlayerCharacterBase> CachedOwner;

	bool bAttacking = false;
	bool bTraceComboAttack = false;
	float ComboAttackRange = 0.f;
	float ComboAttackSpeed = 0.f;
	FCollisionShape ComboTraceShape;
	FVector LastSweepEndLocation = FVector::ZeroVector;
	FVector LastSweepStartLocation = FVector::ZeroVector;
};
