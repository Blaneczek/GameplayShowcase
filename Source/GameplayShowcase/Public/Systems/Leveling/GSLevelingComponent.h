// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GSLevelingComponent.generated.h"

struct FCurrentLevelInfo
{
	FCurrentLevelInfo() = default;
	FCurrentLevelInfo(int32 inXP, int32 inMaxXP, float inXPPercent)
		: XP(inXP), MaxXP(inMaxXP), XPPercent(inXPPercent){}

	int32 XP = 0;
	int32 MaxXP = 0;
	float XPPercent = 0.f;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelChangedSignature, int32 NewLevel);
DECLARE_MULTICAST_DELEGATE_TwoParams(FXPChangedSignature, const FCurrentLevelInfo& /* CurrentLevelInfo */, int32 /* NewLevelsNum */);

class UGSAttributeSetPlayer;
class UGSLevelUpInfo;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSLevelingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGSLevelingComponent();

	// Returns the Leveling component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "GS|Leveling")
	static UGSLevelingComponent* FindLevelingComponent(const AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "GS|Leveling")
	void SetLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "GS|Leveling")
	void AddToXP(int32 InXP);
	
	void LevelUp(int32 InLevel);
	
	FORCEINLINE int32 GetLevel() const;
	FORCEINLINE int32 GetXP() const;

	FCurrentLevelInfo GetCurrentLevelInfo() const;
		
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGSLevelUpInfo> LevelUpInfo;

	FOnLevelChangedSignature OnLevelChangedDelegate;

	FXPChangedSignature OnXPChangedDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Level = 1;
	
private:
	UPROPERTY()
	TObjectPtr<UGSAttributeSetPlayer> AttributeSet;
};

