// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogGameplayShowcase, Log, All);

#define ECC_PlayerChar ECollisionChannel::ECC_GameTraceChannel2
#define ECC_Floor ECollisionChannel::ECC_GameTraceChannel3