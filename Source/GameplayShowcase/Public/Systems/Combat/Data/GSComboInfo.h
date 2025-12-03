// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
#include "GSComboInfo.generated.h"

USTRUCT()
struct FShapeInfoBase
{
	GENERATED_BODY()

	FShapeInfoBase() = default;
	virtual ~FShapeInfoBase() = default;
	
	virtual void MakeShape() {};

	FCollisionShape Shape;	
};

USTRUCT(BlueprintType)
struct FLineTrace : public FShapeInfoBase
{
	GENERATED_BODY()

	virtual void MakeShape() override;
};

USTRUCT(BlueprintType)
struct FSphereTrace : public FShapeInfoBase
{
	GENERATED_BODY()

	virtual void MakeShape() override;

	UPROPERTY(EditAnywhere)
	float Radius = 5.f;
};

USTRUCT(BlueprintType)
struct FCapsuleTrace : public FShapeInfoBase
{
	GENERATED_BODY()

	virtual void MakeShape() override;

	UPROPERTY(EditAnywhere)
	float Radius = 5.f;

	UPROPERTY(EditAnywhere)
	float HalfHeight = 10.f;
};

USTRUCT(BlueprintType)
struct FBoxTrace : public FShapeInfoBase
{
	GENERATED_BODY()

	virtual void MakeShape() override;

	UPROPERTY(EditAnywhere)
	FVector3f HalfExtent = FVector3f(10.f);
};

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSComboInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE FName GetComboSectionNameOnIndex(int32 ComboIndex) const
	{
		return ComboSectionNames.IsValidIndex(ComboIndex) ? ComboSectionNames[ComboIndex] : NAME_None;
	}
	FORCEINLINE const TArray<FName>& GetComboSectionNames() const { return ComboSectionNames; };
	FORCEINLINE int32 GetLastComboIndex() const { return ComboSectionNames.Num() - 1; };	
	FORCEINLINE bool HasValidCombo() const { return !ComboSectionNames.IsEmpty() || !ComboAttackMontage; };
	FORCEINLINE UAnimMontage* GetMontage() const { return ComboAttackMontage; }
	FORCEINLINE float GetAttackRange() const { return AttackRange; }
	FORCEINLINE FCollisionShape GetTraceShape();
	
protected:
	/** AnimMontage that will play for combo attacks */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> ComboAttackMontage = nullptr;

	/** Names of the AnimMontage sections that correspond to each stage of the combo attack */
	UPROPERTY(EditAnywhere)
	TArray<FName> ComboSectionNames;
	
	UPROPERTY(EditAnywhere)
	TInstancedStruct<FShapeInfoBase> TraceShapeInfo;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin = 1.f))
	float AttackRange = 100.f;

private:
	bool bShapeSet = false;
};
