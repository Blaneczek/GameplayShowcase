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
#include "Systems/AbilitySystem/GSGameplayTags.h"


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
		GSInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AGSPlayerController::WSADMovementEnded);
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
	
	PlayerCharacter = Cast<AGSPlayerCharacterBase>(aPawn);
	CachedHUD = GetHUD<AGSHUD>();
	InitializeHUD();
}

void AGSPlayerController::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	const FRotator YawRotation{0, GetControlRotation().Yaw, 0};
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (!CheckIfCanMove())
    {
		const FVector DesiredMoveDirection = (ForwardDirection * MovementVector.X + RightDirection * MovementVector.Y).GetSafeNormal();
		RotateToDesiredDirection(DesiredMoveDirection);
		return;
    }
	
	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.X);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.Y);

	// Rotate camera a little if input is right/left
	if (FMath::Abs(MovementVector.Y) > 0.f)
	{
		AddYawInput(MovementVector.Y * FreeYawRotationSpeed);
	}
}

void AGSPlayerController::StopOngoingMovement()
{
	if (PlayerCharacter)
	{
		bWSADMovement = true;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, PlayerCharacter->GetActorLocation());
	}
}

void AGSPlayerController::AutoMove()
{
	if (bWSADMovement || !CheckIfCanMove())
	{
		return;
	}
	// If GridItemProxy exists
	if (OnLeftMouseButtonDown.IsBound())
	{
		return;
	}
	
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (CursorHit.bBlockingHit && PlayerCharacter)
	{
		const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		const float DistanceToDestination = (PlayerLocation - CursorHit.Location).SquaredLength();			
		if (DistanceToDestination >= (AutoMoveAcceptanceRadius * AutoMoveAcceptanceRadius))
		{
			const FVector Direction = (CursorHit.Location - PlayerLocation).GetSafeNormal();
            PlayerCharacter->AddMovementInput(Direction);
		}
	}	
}

void AGSPlayerController::StopAutoMove()
{	
	if (bWSADMovement || !CheckIfCanMove())
	{
		return;
	}
	if (OnLeftMouseButtonDown.IsBound())
	{
		if (OnLeftMouseButtonDown.Execute())
		{
			return;
		}		
	}
	
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (CursorHit.bBlockingHit && CursorHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CursorHitEffect, CursorHit.Location);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CursorHit.Location);
	}	
}

void AGSPlayerController::WSADMovementEnded()
{
	bWSADMovement = false;
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
	if (OnRightMouseButtonDown.IsBound())
	{
		if (OnRightMouseButtonDown.Execute())
		{
			return;
		}
	}
	
	bCanLook = Value.Get<bool>();
	bCanLook ?
		CurrentMouseCursor = MouseCursors[EGameplayCursorType::CameraLook] :
		CurrentMouseCursor = MouseCursors[EGameplayCursorType::Default];	
}

void AGSPlayerController::CameraZoom(const FInputActionValue& Value)
{	
	if (PlayerCharacter)
	{
		const float ZoomValue = Value.Get<float>();
		PlayerCharacter->GetCameraArm()->AddZoomInput(ZoomValue * CameraZoomSpeed);
	}
}

void AGSPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC())
	{
		GetASC()->AbilityInputTagPressed(InputTag);
	}
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
	if (PlayerCharacter)
	{
		PlayerCharacter->PickUpItem();
	}
}

bool AGSPlayerController::CheckIfCanMove()
{
	if (GetASC())
	{		
		return !GetASC()->HasMatchingGameplayTag(GSGameplayTags::Status_Movement_Forbidden.GetTag());
	}
	return true;
}

void AGSPlayerController::RotateToDesiredDirection(const FVector& DesiredMoveDirection)
{
	if (DesiredMoveDirection.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		const FRotator TargetRotation = DesiredMoveDirection.Rotation();
		FRotator NewRotation = FMath::RInterpTo(
			PlayerCharacter->GetActorRotation(),
			TargetRotation,
			GetWorld()->GetDeltaSeconds(),
			10.f  // e.g. 10–15
		);

		PlayerCharacter->SetActorRotation(NewRotation);
	}
}

void AGSPlayerController::InitializeHUD()
{
	if (CachedHUD)
	{
		CachedHUD->InitializeOverlayWidget(this, PlayerCharacter, PlayerCharacter->GetAbilitySystemComponent(), PlayerCharacter->GetAttributeSet());
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
