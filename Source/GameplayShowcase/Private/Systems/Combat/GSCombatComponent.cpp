// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/GSCombatComponent.h"

#include "KismetTraceUtils.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "Systems/Combat/GSDamageable.h"

UGSCombatComponent::UGSCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UGSCombatComponent* UGSCombatComponent::FindCombatComponent(AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UGSCombatComponent>() : nullptr;
}

void UGSCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAttacking)
	{
		if (bTraceComboAttack)
        {
        	DoComboAttackTrace();
        }
	}
}

void UGSCombatComponent::StartComboAttackTrace(float AttackRange, const FCollisionShape& TraceShape)
{
	const auto* Owner = CachedOwner.Get();
	if (!Owner)
	{
		return;
	}
	
	ComboTraceShape = TraceShape;
    ComboAttackRange = AttackRange;

	LastSweepStartLocation = Owner->GetMesh()->GetSocketLocation("Weapon_1H");
	const FVector ForwardDirection = Owner->GetMesh()->GetSocketRotation("Weapon_1H").RotateVector(FVector::ForwardVector).GetSafeNormal();
	LastSweepEndLocation = LastSweepStartLocation + ForwardDirection * ComboAttackRange;
		
	bTraceComboAttack = true;
}

void UGSCombatComponent::StopComboAttackTrace()
{
	bTraceComboAttack = false;
	
}

void UGSCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwner = Cast<AGSPlayerCharacterBase>(GetOwner());
}

void UGSCombatComponent::DoComboAttackTrace()
{
	const auto* Owner = CachedOwner.Get();
	if (!Owner)
	{
		return;
	}
	const FVector Start = Owner->GetMesh()->GetSocketLocation("Weapon_1H");
	const FVector ForwardDirection = Owner->GetMesh()->GetSocketRotation("Weapon_1H").RotateVector(FVector::ForwardVector).GetSafeNormal();
	const FVector End = Start + ForwardDirection * ComboAttackRange;

	const float EndsDistance = FVector::Dist(LastSweepEndLocation, End);
	const float MaxGap = 30.f;
	if (EndsDistance > MaxGap)
	{
		const int32 Times = static_cast<int32>(EndsDistance / MaxGap);
		FillSweepGaps(Times, LastSweepStartLocation, Start, LastSweepEndLocation, End);
	}
	
	SweepShape(Start, End);
	LastSweepEndLocation = End;
}

void UGSCombatComponent::SweepShape(const FVector& From, const FVector& To, bool bGap)
{
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CachedOwner.Get());

	TArray<FHitResult> OutHits;
	const bool bHit = GetWorld()->SweepMultiByObjectType(OutHits, From, To, FQuat::Identity, ObjectParams, ComboTraceShape, QueryParams);
	if (bDebugTrace)
    {
    	DrawDebugShape(From, To, bHit, OutHits, bGap);
    }
	if (!bHit)
	{
		return;
	}
		
	for (const FHitResult& Hit : OutHits)
	{
		if (IGSDamageable* Damageable = Cast<IGSDamageable>(Hit.GetActor()))
		{		
			UE_LOG(LogTemp, Warning, TEXT("attack hit"));
		}
	}	
}

void UGSCombatComponent::DrawDebugShape(const FVector& Start, const FVector& End, bool bHit, const TArray<FHitResult>& OutHits, bool bGap)
{
	FLinearColor TraceColor = FLinearColor::Red;
	if (bGap)
	{
		TraceColor = FLinearColor::Blue;
	}
	
	if (ComboTraceShape.IsCapsule())
	{
		DrawDebugCapsuleTraceMulti(GetWorld(), Start, End, ComboTraceShape.GetCapsuleRadius(), ComboTraceShape.GetCapsuleHalfHeight(),
			EDrawDebugTrace::ForDuration, bHit, OutHits, TraceColor , FLinearColor::Green, 5.f);
	}
	if (ComboTraceShape.IsSphere())
	{
		DrawDebugSphereTraceMulti(GetWorld(), Start, End, ComboTraceShape.GetSphereRadius(), EDrawDebugTrace::ForDuration, bHit,
			OutHits, TraceColor , FLinearColor::Green, 5.f);
	}
	if (ComboTraceShape.IsBox())
	{
		DrawDebugBoxTraceMulti(GetWorld(), Start, End, ComboTraceShape.GetExtent() * 0.5f, ComboTraceShape.GetExtent().Rotation(),
			EDrawDebugTrace::ForDuration, bHit, OutHits, TraceColor, FLinearColor::Green, 5.f);
	}
	if (ComboTraceShape.IsLine())
	{
		DrawDebugLineTraceMulti(GetWorld(), Start, End, EDrawDebugTrace::ForDuration, bHit, OutHits,
			TraceColor,FLinearColor::Green, 5.f);
	}
	
}

void UGSCombatComponent::FillSweepGaps(int32 Times, const FVector& LastStart, const FVector& Start, const FVector& LastEnd, const FVector& End)
{
	if (Times <= 0)
	{
		return;
	}
	
	for (int32 i = 1; i <= Times; ++i)
	{
		const float Alpha = static_cast<float>(i) / (Times + 1);
		const FVector SubStart = FMath::Lerp(LastStart, Start, Alpha);
		const FVector SubEnd = FMath::Lerp(LastEnd, End, Alpha);
		SweepShape(SubStart, SubEnd, true);
	}
}


