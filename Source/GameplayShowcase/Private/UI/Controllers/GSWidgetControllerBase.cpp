// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Public/UI/Controllers/GSWidgetControllerBase.h"

void UGSWidgetControllerBase::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	Character = WCParams.Character;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}
