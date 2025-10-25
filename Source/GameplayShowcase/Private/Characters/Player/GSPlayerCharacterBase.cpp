// Copyright (c) 2025 Dawid Szoldra. All rights reserved.

#include "Public/Characters/Player/GSPlayerCharacterBase.h"
#include "AbilitySystem/GSAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/GSAttributeSetPlayer.h"
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
}

void AGSPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
	InitializeAttributes();
	AddAbilities();
}

void AGSPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGSPlayerCharacterBase::InitializeAttributes()
{
	for (const auto& EffectClass : DefaultAttributesEffectClasses)
	{
		if (EffectClass)
		{
			const FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
			const FGameplayEffectSpecHandle GameplayEffectSpec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1, ContextHandle);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec.Data.Get());
		}
	}
}

void AGSPlayerCharacterBase::AddAbilities()
{
	//TODO
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

