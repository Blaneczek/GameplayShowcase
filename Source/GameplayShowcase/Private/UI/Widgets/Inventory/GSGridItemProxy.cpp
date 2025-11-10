// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridItemProxy.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Player/GSPlayerController.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"


void UGSGridItemProxy::InitProxy(UCanvasPanelSlot* inSlot, TWeakObjectPtr<UCanvasPanel> Canvas, const FVector2D& inWidgetSize, const FItemSize& inProxySize)
{
	WidgetSize = inWidgetSize;
	ProxySize = inProxySize;
	CanvasSlot = inSlot;
	CanvasPanel = Canvas;
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	StartDragging();

	if (AGSPlayerController* PlayerController = Cast<AGSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PlayerController->OnLeftMouseButtonDown.BindUObject(this, &UGSGridItemProxy::HandleProxyAction);
		PlayerController->OnRightMouseButtonDown.BindUObject(this, &UGSGridItemProxy::RemoveProxy);
	}
}

void UGSGridItemProxy::StartDragging()
{
	if (!CanvasPanel.IsValid() || !CanvasSlot)
	{
		return;		
	}
	
	CanvasSize = CanvasPanel.Get()->GetCachedGeometry().GetLocalSize();
	CanvasSlot->SetSize(WidgetSize);
	CanvasSlot->SetPosition(UWidgetLayoutLibrary::GetMousePositionOnViewport(this) - (WidgetSize * 0.5f));

	bIsDragging = true;	
}

void UGSGridItemProxy::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (bIsDragging && CanvasSlot)
	{
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		FVector2D NewPosition = MousePosition - (WidgetSize * 0.5f);
		NewPosition.X = FMath::Clamp(NewPosition.X, 0, CanvasSize.X);
		NewPosition.Y = FMath::Clamp(NewPosition.Y, 0, CanvasSize.Y);
		
		CanvasSlot->SetPosition(NewPosition);
	}
}

void UGSGridItemProxy::NativeDestruct()
{
	Super::NativeDestruct();

	if (AGSPlayerController* PlayerController = Cast<AGSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PlayerController->OnLeftMouseButtonDown.Unbind();
		PlayerController->OnRightMouseButtonDown.Unbind();
	}
}

bool UGSGridItemProxy::RemoveProxy()
{
	if (UGSInventoryMenuWidgetController* Controller = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		Controller->CallOnGridItemProxyStatusChanged(false, ProxySize);	
	}
	bIsDragging = false;
	RemoveFromParent();
	
	return true;
}

bool UGSGridItemProxy::HandleProxyAction()
{
	if (UGSInventoryMenuWidgetController* Controller = UGSBlueprintFunctionLibrary::GetInventoryMenuWidgetController(this))
	{
		Controller->CallOnGridItemProxyStatusChanged(false, ProxySize);	
	}
	bIsDragging = false;
	// TODO: Move GridItem
	RemoveFromParent();

	return true;
}


