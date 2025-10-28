// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#include "Public/Characters/Player/GSPlayerCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GSGameplayTags.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Camera/GSCameraComponent.h"
#include "Player/Camera/GSSpringArmComponent.h"


AGSPlayerCharacterBase::AGSPlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CameraArm = CreateDefaultSubobject<UGSSpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UGSCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm);

	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UGSAttributeSetPlayer>(TEXT("AttributeSet"));
}

void AGSPlayerCharacterBase::InitAbilityActorInfo()
{
	checkf(IsValid(AbilitySystemComponent), TEXT("AGSPlayerCharacterBase::InitAbilityActorInfo || AbilitySystemComponent is not valid"));
	checkf(IsValid(AttributeSet), TEXT("AGSPlayerCharacterBase::InitAbilityActorInfo || AttributeSet is not valid"));

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AbilitySystemComponent->RegisterGameplayTagEvent(GSGameplayTags::Status_ST_Consuming.GetTag(), EGameplayTagEventType::NewOrRemoved)
							.AddUObject(this, &AGSPlayerCharacterBase::OnStaminaConsumingTagChanged);
}

void AGSPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
	InitializeAttributes();
	AddAbilities(StartupAbilities);
}

void AGSPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	DefaultMovementSpeed = GetCharacterMovement()->GetMaxSpeed();
}

void AGSPlayerCharacterBase::InitializeAttributes()
{
	for (const auto& EffectClass : DefaultAttributesEffectClasses)
	{
		ApplySimpleGameplayEffectToSelf(EffectClass, 1.f);
	}
}

void AGSPlayerCharacterBase::AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	if (UGSAbilitySystemComponent* GSASC = Cast<UGSAbilitySystemComponent>(AbilitySystemComponent))
	{
		GSASC->AddCharacterAbilities(Abilities);
	}
}

void AGSPlayerCharacterBase::ApplySimpleGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level)
{
	if (EffectClass)
	{
		const FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
        const FGameplayEffectSpecHandle GameplayEffectSpec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, ContextHandle);
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data.Get());
	}
}

void AGSPlayerCharacterBase::CheckIfCharacterIsMoving()
{
	if (FMath::IsNearlyZero(GetVelocity().SquaredLength(), 1.f))
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GSGameplayTags::Event_Character_Movement_Stopped, FGameplayEventData());
	}
}

void AGSPlayerCharacterBase::OnStaminaConsumingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// Consuming Tag was removed
	if (NewCount == 0)
	{	
		GetWorldTimerManager().SetTimer(
			StaminaRegenTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				ApplySimpleGameplayEffectToSelf(STRegenEffectClass, 1.f);
			}),
			StaminaRegenDelay,
			false
		);
	}
	// Consuming Tag was added
	else
	{
		GetWorldTimerManager().ClearTimer(StaminaRegenTimerHandle);
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(GSGameplayTags::Status_ST_Regen.GetTag()));
	}
}

UGSSpringArmComponent* AGSPlayerCharacterBase::GetCameraArm() const
{
	return CameraArm;
}

UAbilitySystemComponent* AGSPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AGSPlayerCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

void AGSPlayerCharacterBase::SetMovementSpeed(bool bSprint, float NewSpeed)
{
	if (bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
		GetWorldTimerManager().SetTimer(IsMovingTimerHandle, this, &AGSPlayerCharacterBase::CheckIfCharacterIsMoving, 0.25f, true);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMovementSpeed;
		if (IsMovingTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(IsMovingTimerHandle);
		}
	}
}

