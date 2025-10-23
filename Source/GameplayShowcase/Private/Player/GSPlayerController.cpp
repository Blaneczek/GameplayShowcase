// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/Player/GSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Characters/Player/GSPlayerCharacterBase.h"
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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AGSPlayerController::StopOngoingMovement);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGSPlayerController::Move);
		EnhancedInputComponent->BindAction(AutoMoveAction, ETriggerEvent::Triggered, this, &AGSPlayerController::AutoMove);
		EnhancedInputComponent->BindAction(AutoMoveAction, ETriggerEvent::Completed, this, &AGSPlayerController::StopAutoMove);

		// Camera
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGSPlayerController::Look);
		EnhancedInputComponent->BindAction(EnableLookAction, ETriggerEvent::Started, this, &AGSPlayerController::EnableLook);
		EnhancedInputComponent->BindAction(EnableLookAction, ETriggerEvent::Completed, this, &AGSPlayerController::EnableLook);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AGSPlayerController::CameraZoom);
	}
}

void AGSPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	CachedPlayerCharacter = CastChecked<AGSPlayerCharacterBase>(aPawn);
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
