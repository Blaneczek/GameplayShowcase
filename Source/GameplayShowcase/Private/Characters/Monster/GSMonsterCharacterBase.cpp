// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/Characters/Monster/GSMonsterCharacterBase.h"

#include "Systems/AbilitySystem/GSAbilitySystemComponent.h"
#include "Systems/AbilitySystem/AttributeSets/GSAttributeSetBase.h"


AGSMonsterCharacterBase::AGSMonsterCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UGSAttributeSetBase>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGSMonsterCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AGSMonsterCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

void AGSMonsterCharacterBase::BeginPlay()
{
	Super::BeginPlay();	
}

