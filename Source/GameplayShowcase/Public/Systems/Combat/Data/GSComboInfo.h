// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GSComboInfo.generated.h"


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
	
protected:
	/** AnimMontage that will play for combo attacks */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> ComboAttackMontage = nullptr;

	/** Names of the AnimMontage sections that correspond to each stage of the combo attack */
	UPROPERTY(EditAnywhere)
	TArray<FName> ComboSectionNames;
};
