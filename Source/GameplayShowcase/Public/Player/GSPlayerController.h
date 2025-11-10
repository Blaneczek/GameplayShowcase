// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/HUD/GSHUD.h"
#include "GSPlayerController.generated.h"

class AGSHUD;
class UGSAbilitySystemComponent;
class UGSInputConfig;
struct FGameplayTag;
class UNiagaraSystem;
class AGSPlayerCharacterBase;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

DECLARE_DELEGATE_RetVal(bool, FOnMouseButtonDownSignature);

UENUM(BlueprintType)
enum class EGameplayCursorType : uint8
{
	None,
	Default,
	CameraLook
};

/**
 * 
 */
UCLASS()
class GAMEPLAYSHOWCASE_API AGSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGSPlayerController();

	virtual void Tick(float DeltaTime) override;

	FOnMouseButtonDownSignature OnLeftMouseButtonDown;
	FOnMouseButtonDownSignature	OnRightMouseButtonDown;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;

	UPROPERTY(EditAnywhere, Category ="Input")
	TObjectPtr<UInputMappingContext> GSContext;

	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> AutoMoveAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> EnableLookAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> CameraZoomAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> CharacterMenuAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> InventoryMenuAction;
	UPROPERTY(EditAnywhere, Category ="Input|Action")
	TObjectPtr<UInputAction> PickUpAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UGSInputConfig> InputConfig;
	
	UPROPERTY(EditAnywhere, Category ="Input|CameraDefaults")
	float YawRotationSpeed = 2.f;
	UPROPERTY(EditAnywhere, Category ="Input|CameraDefaults")
	float PitchRotationSpeed = 2.f;
	UPROPERTY(EditAnywhere, Category="Input|CameraDefaults")
	float CameraZoomSpeed = 100.f;
	UPROPERTY(EditAnywhere, Category="Input|CameraDefaults")
	float FreeYawRotationSpeed = 0.1f;

	UPROPERTY(EditAnywhere, Category="Input|MovementDefaults")
	float AutoMoveAcceptanceRadius = 15.f;

	UPROPERTY(EditAnywhere, Category="Cursor")
	TMap<EGameplayCursorType, TEnumAsByte<EMouseCursor::Type>> MouseCursors;
	UPROPERTY(EditAnywhere, Category="Cursor")
	TObjectPtr<UNiagaraSystem> CursorHitEffect;
	
private:
	void Move(const FInputActionValue& Value);
	void StopOngoingMovement();
	void AutoMove();
	void StopAutoMove();
	
	void Look(const FInputActionValue& Value);
	void EnableLook(const FInputActionValue& Value);
	void CameraZoom(const FInputActionValue& Value);
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void PickUp();
	
	/** UI **/
	void InitializeHUD();
	
	template <typename T> requires std::is_base_of_v<UGSWidgetBase, T>
	void OpenOrCloseMenuByType();
	/*******/
	
	UGSAbilitySystemComponent* GetASC();
		
	UPROPERTY()
	TObjectPtr<AGSPlayerCharacterBase> CachedPlayerCharacter;
	
	UPROPERTY()
	TObjectPtr<AGSHUD> CachedHUD;
	
	UPROPERTY()
	TObjectPtr<UGSAbilitySystemComponent> AbilitySystemComponent;
	
	bool bCanLook = false;
};

template <typename T> requires std::is_base_of_v<UGSWidgetBase, T>
void AGSPlayerController::OpenOrCloseMenuByType()
{
	if (CachedHUD)
	{
		CachedHUD->OpenOrCloseMenuByType<T>();
	}
}
