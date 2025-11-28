// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#include "Public/Characters/Player/GSPlayerCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Systems/AbilitySystem/GSAbilitySystemComponent.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Camera/GSCameraComponent.h"
#include "Player/Camera/GSSpringArmComponent.h"
#include "Systems/AbilitySystem/GSGameplayTags.h"
#include "Systems/Combat/GSCombatComponent.h"
#include "Systems/Inventory/GSEquipmentComponent.h"
#include "Systems/Inventory/GSInventoryComponent.h"
#include "Systems/Leveling/GSLevelingComponent.h"


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
	LevelingComponent = CreateDefaultSubobject<UGSLevelingComponent>(TEXT("LevelingComponent"));
	InventoryComponent = CreateDefaultSubobject<UGSInventoryComponent>(TEXT("InventoryComponent"));
	EquipmentComponent = CreateDefaultSubobject<UGSEquipmentComponent>(TEXT("EquipmentComponent"));
	CombatComponent = CreateDefaultSubobject<UGSCombatComponent>(TEXT("CombatComponent"));
}

void AGSPlayerCharacterBase::InitAbilityActorInfo()
{
	checkf(IsValid(AbilitySystemComponent), TEXT("AGSPlayerCharacterBase::InitAbilityActorInfo || AbilitySystemComponent is not valid"));
	checkf(IsValid(AttributeSet), TEXT("AGSPlayerCharacterBase::InitAbilityActorInfo || AttributeSet is not valid"));

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AbilitySystemComponent->RegisterGameplayTagEvent(GSGameplayTags::Status_ST_Consuming.GetTag(), EGameplayTagEventType::NewOrRemoved)
							.AddUObject(this, &AGSPlayerCharacterBase::OnSTConsumingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(GSGameplayTags::Status_HP_Consuming.GetTag(), EGameplayTagEventType::NewOrRemoved)
							.AddUObject(this, &AGSPlayerCharacterBase::OnHPConsumingTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(GSGameplayTags::Status_PE_Consuming.GetTag(), EGameplayTagEventType::NewOrRemoved)
							.AddUObject(this, &AGSPlayerCharacterBase::OnPEConsumingTagChanged);
}

void AGSPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
	InitializeAttributes();
	AddAbilities(GeneralAbilities);
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

void AGSPlayerCharacterBase::OnSTConsumingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	OnConsumingTagChanged(Tag, NewCount, STRegenTimerHandle, STRegenEffectClass, GSGameplayTags::Status_ST_Regen.GetTag());
}

void AGSPlayerCharacterBase::OnHPConsumingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	OnConsumingTagChanged(Tag, NewCount, HPRegenTimerHandle, HPRegenEffectClass, GSGameplayTags::Status_HP_Regen.GetTag());
}

void AGSPlayerCharacterBase::OnPEConsumingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	OnConsumingTagChanged(Tag, NewCount, PERegenTimerHandle, PERegenEffectClass, GSGameplayTags::Status_PE_Regen.GetTag());
}

void AGSPlayerCharacterBase::OnConsumingTagChanged(const FGameplayTag Tag, int32 NewCount,
	FTimerHandle& TimerHandle, TSubclassOf<UGameplayEffect> EffectClass, const FGameplayTag& RegenTag)
{
	if (!EffectClass)
	{
		return;
	}
	
	// Consuming Tag was removed
	if (NewCount == 0)
	{	
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this, EffectClass]()
			{
				ApplySimpleGameplayEffectToSelf(EffectClass, 1.f);
			}),
			RegenDelay,
			false
		);
	}
	// Consuming Tag was added
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(RegenTag));
	}
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

void AGSPlayerCharacterBase::PickUpItem()
{
	InventoryComponent->TryAddItem();
}

