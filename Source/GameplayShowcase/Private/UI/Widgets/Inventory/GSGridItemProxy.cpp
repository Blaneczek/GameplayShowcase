// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "UI/Widgets/Inventory/GSGridItemProxy.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Player/GSPlayerController.h"
#include "Systems/AbilitySystem/GSBlueprintFunctionLibrary.h"
#include "UI/Controllers/GSInventoryMenuWidgetController.h"
#include "UI/Widgets/Inventory/GSGridItem.h"


void UGSGridItemProxy::InitProxy(UTexture2D* ItemIcon, UGSGridItem* GridItem, UCanvasPanelSlot* inSlot, TWeakObjectPtr<UCanvasPanel> Canvas, const FVector2D& inWidgetSize, const FItemSize& inProxySize)
{
	if (ItemIcon)
	{
		ItemProxyIcon->SetBrushFromTexture(ItemIcon);
	}
	
	GridItemRef = GridItem;
	WidgetSize = inWidgetSize;
	ProxySize = inProxySize;
	CanvasSlot = inSlot;
	CanvasPanel = Canvas;
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	StartDragging();

	if (AGSPlayerController* PlayerController = Cast<AGSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PlayerController->OnLeftMouseButtonDown.BindUObject(this, &UGSGridItemProxy::RemoveProxy);
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
	CanvasSlot->SetZOrder(100);
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


