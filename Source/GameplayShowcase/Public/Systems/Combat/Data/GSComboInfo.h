// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GSComboInfo.generated.h"


USTRUCT()
struct FComboAction
{
	GENERATED_BODY()

	FComboAction() = default;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditAnywhere)
	FText DebugString;

	bool IsValid() const { return Montage != nullptr; }
};
/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API UGSComboInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FComboAction GetComboAction(int32 ComboIndex) const;
	FORCEINLINE int32 GetLastIndex() const { return ComboActions.Num() - 1; };
	
	bool HasValidCombo() const;
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<FComboAction> ComboActions;
};
