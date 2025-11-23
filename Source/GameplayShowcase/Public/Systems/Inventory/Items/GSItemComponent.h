// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/GSItemDefinition.h"
#include "Components/SphereComponent.h"
#include "GSItemComponent.generated.h"

class UGSItemDataSubsystem;
class UGSItemsInfo;

DECLARE_DELEGATE(FOnItemDefinitionSet);

/**
 * 
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSHOWCASE_API UGSItemComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UGSItemComponent();

	// Returns the Item component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category="GS|ItemComponent")
	static UGSItemComponent* FindItemComponent(const AActor* Actor);

	FORCEINLINE const FItemDefinition& GetItemDefinitionRef() const { return ItemDefinition; }
	FORCEINLINE FItemDefinition& GetItemDefinitionRefMutable() { return ItemDefinition; }

	FORCEINLINE void SetItemTag(const FGameplayTag& Tag) { ItemTag = Tag; }
	FORCEINLINE void EnableRandomizer() { bRandomizeItem = true; }
	
	void MoveItemDefinition(FItemDefinition&& Def);
	void LoadDefinition();
	
	FOnItemDefinitionSet OnItemDefinitionSet;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="GS|ItemComponent")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, Category="GS|ItemComponent")
	FString PickupMessage;
	
private:
	UFUNCTION()
	void SetItemDefinition(UGSItemDataSubsystem* ItemDataSubsystem);
	
	UFUNCTION()
    void PickUpZoneEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                           bool bFromSweep, const FHitResult& SweepResult);  
    UFUNCTION()
    void PickUpZoneLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void HandlePickUpOverlap(AActor* OtherActor, bool bEntered);

	void RandomizeItem();
	
	UPROPERTY()
	FItemDefinition ItemDefinition;
	
	bool bRandomizeItem = false;
	bool bDefinitionSet = false;		
};
