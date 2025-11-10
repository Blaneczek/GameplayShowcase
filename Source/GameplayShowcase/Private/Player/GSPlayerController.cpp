// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/Player/GSPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Systems/AbilitySystem/GSAbilitySystemComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Input/GSInputComponent.h"
#include "Player/Camera/GSSpringArmComponent.h"


AGSPlayerController::AGSPlayerController()
{
}

void AGSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	CurrentMouseCursor = MouseCursors[EGameplayCursorType::Default];
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AGSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedInputSubsystem->AddMappingContext(GSContext, 0);
	}

	if (UGSInputComponent* GSInputComponent = Cast<UGSInputComponent>(InputComponent))
	{
		// Movement
		GSInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGSPlayerController::Move);
		GSInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AGSPlayerController::StopOngoingMovement);
		GSInputComponent->BindAction(AutoMoveAction, ETriggerEvent::Triggered, this, &AGSPlayerController::AutoMove);
		GSInputComponent->BindAction(AutoMoveAction, ETriggerEvent::Completed, this, &AGSPlayerController::StopAutoMove);

		// Camera
		GSInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGSPlayerController::Look);
		GSInputComponent->BindAction(EnableLookAction, ETriggerEvent::Started, this, &AGSPlayerController::EnableLook);
		GSInputComponent->BindAction(EnableLookAction, ETriggerEvent::Completed, this, &AGSPlayerController::EnableLook);
		GSInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AGSPlayerController::CameraZoom);

		// Abilities
		GSInputComponent->BindAbilityActions(InputConfig, this, &AGSPlayerController::AbilityInputTagPressed,
																		  &AGSPlayerController::AbilityInputTagHeld,
																		  &AGSPlayerController::AbilityInputTagReleased);

		// Widgets
		GSInputComponent->BindAction(CharacterMenuAction, ETriggerEvent::Started, this, &AGSPlayerController::OpenOrCloseMenuByType<UGSCharacterMenuWidget>);
		GSInputComponent->BindAction(InventoryMenuAction, ETriggerEvent::Started, this, &AGSPlayerController::OpenOrCloseMenuByType<UGSInventoryMenuWidget>);

		//Others
		GSInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &AGSPlayerController::PickUp);
	}
}

void AGSPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	CachedPlayerCharacter = Cast<AGSPlayerCharacterBase>(aPawn);
	CachedHUD = GetHUD<AGSHUD>();
	InitializeHUD();
}

void AGSPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation{0, GetControlRotation().Yaw, 0};

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.X);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.Y);

	// Rotate a little if input is right/left
	if (FMath::Abs(MovementVector.Y) > 0.f)
	{
		AddYawInput(MovementVector.Y * FreeYawRotationSpeed);
	}
}

void AGSPlayerController::StopOngoingMovement()
{
	if (CachedPlayerCharacter)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedPlayerCharacter->GetActorLocation());
	}	
}

void AGSPlayerController::AutoMove()
{
	// If GridItemProxy is created
	if (OnLeftMouseButtonDown.IsBound())
	{
		return;
	}
	
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (CursorHit.bBlockingHit && CachedPlayerCharacter)
	{
		const FVector PlayerLocation = CachedPlayerCharacter->GetActorLocation();
		const float DistanceToDestination = (PlayerLocation - CursorHit.Location).SquaredLength();			
		if (DistanceToDestination >= (AutoMoveAcceptanceRadius * AutoMoveAcceptanceRadius))
		{
			const FVector Direction = (CursorHit.Location - PlayerLocation).GetSafeNormal();
            CachedPlayerCharacter->AddMovementInput(Direction);
		}
	}
}

void AGSPlayerController::StopAutoMove()
{
	if (OnLeftMouseButtonDown.IsBound() && OnLeftMouseButtonDown.Execute())
	{
		return;
	}
	
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (CursorHit.bBlockingHit && CursorHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CursorHitEffect, CursorHit.Location);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CursorHit.Location);
	}
}

void AGSPlayerController::Look(const FInputActionValue& Value)
{
	if (!bCanLook)
	{
		return;
	}
	
	const FVector2D LookAxisVector = Value.Get<FVector2D>();	
	AddYawInput(LookAxisVector.X * YawRotationSpeed);
	AddPitchInput(LookAxisVector.Y * PitchRotationSpeed);
}

void AGSPlayerController::EnableLook(const FInputActionValue& Value)
{
	if (OnRightMouseButtonDown.IsBound() && OnRightMouseButtonDown.Execute())
	{
		return;
	}
	
	bCanLook = Value.Get<bool>();
	bCanLook ? CurrentMouseCursor = MouseCursors[EGameplayCursorType::CameraLook] : CurrentMouseCursor = MouseCursors[EGameplayCursorType::Default];	
}

void AGSPlayerController::CameraZoom(const FInputActionValue& Value)
{
	const float ZoomValue = Value.Get<float>();

	if (CachedPlayerCharacter)
	{
		CachedPlayerCharacter->GetCameraArm()->AddZoomInput(ZoomValue * CameraZoomSpeed);
	}
}

void AGSPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	
}

void AGSPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}
}

void AGSPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC())
	{
		GetASC()->AbilityInputTagHeld(InputTag);
	}
}

void AGSPlayerController::PickUp()
{
	if (CachedPlayerCharacter)
	{
		CachedPlayerCharacter->PickUpItem();
	}
}

void AGSPlayerController::InitializeHUD()
{
	if (CachedHUD)
	{
		CachedHUD->InitializeOverlayWidget(this, CachedPlayerCharacter, CachedPlayerCharacter->GetAbilitySystemComponent(), CachedPlayerCharacter->GetAttributeSet());
	}
}

UGSAbilitySystemComponent* AGSPlayerController::GetASC()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = Cast<UGSAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	
	return AbilitySystemComponent;
}
