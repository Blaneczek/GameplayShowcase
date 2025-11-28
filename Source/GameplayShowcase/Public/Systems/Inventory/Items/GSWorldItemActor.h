// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GSWorldItemActor.generated.h"

class UWidgetComponent;
struct FItemDefinition;
class UGSItemComponent;

UCLASS()
class GAMEPLAYSHOWCASE_API AGSWorldItemActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGSWorldItemActor();

	void MoveItemDefinition(FItemDefinition&& Def);

	FORCEINLINE UGSItemComponent* GetItemComponent() { return ItemComponent; }

	/** If set to true, ItemComponent will load data automatically in BeginPlay.
	 *	ItemTag in ItemComponent must already be set.
	 */
	UPROPERTY(EditAnywhere)
	bool bLoadDataAutomatically = false;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ItemNameSet(const FText& Name, int32 UpgradeLevel = 99);
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootScene;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGSItemComponent> ItemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ItemMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> NameWidget; 
	
private:
	UFUNCTION()
	void OnItemStoppedFalling(UPrimitiveComponent* SleepingComponent, FName BoneName);
	
	void SetItemMesh();
	void OnDefinitionSet();

	
};
