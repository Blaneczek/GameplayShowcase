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

	void SetItemMesh();
	void SetItemDefinition(FItemDefinition&& Def);

	UFUNCTION(BlueprintImplementableEvent)
	void ItemNameSet(const FText& Name);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	void OnDefinitionSet();
};
