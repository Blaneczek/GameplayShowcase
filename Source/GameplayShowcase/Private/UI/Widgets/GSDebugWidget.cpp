// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/GSDebugWidget.h"

#include "Characters/Player/GSPlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/Leveling/GSLevelingComponent.h"

void UGSDebugWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	LevelingComponent = UGSLevelingComponent::FindLevelingComponent(UGameplayStatics::GetPlayerCharacter(this, 0));
}
