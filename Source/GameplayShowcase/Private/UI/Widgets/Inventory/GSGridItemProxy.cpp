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


void UGSGridItemProxy::InitProxy(UTexture2D* ItemIcon, UGSGridItem* GridItem, UCanvasPanelSlot* InSlot, TWeakObjectPtr<UCanvasPanel> InCanvas, const FVector2D& InWidgetSize, const FItemSize& InProxySize)
{
	// Must be not hit testable so that other widgets can be clicked
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!ItemIcon || !InSlot)
	{
		RemoveProxy();
		return;
	}
	
	ItemProxyIcon->SetBrushFromTexture(ItemIcon);
	GridItemRef = GridItem;
	WidgetSize = InWidgetSize;
	ProxySize = InProxySize;
	CanvasSlot = InSlot;
	CanvasPanel = InCanvas;

	// Widget is not hit testable, but we need to be able to remove widget when right mouse button is down 
	if (AGSPlayerController* PlayerController = Cast<AGSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		//TODO: LeftMouseButton - if on Level, remove item from inventory
		//PlayerController->OnLeftMouseButtonDown.BindUObject(this, &UGSGridItemProxy::RemoveProxy);
		PlayerController->OnRightMouseButtonDown.BindUObject(this, &UGSGridItemProxy::RemoveProxy);
	}	
    InitDragging();
}

void UGSGridItemProxy::InitDragging()
{
	if (!CanvasPanel.IsValid())
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

	if (bIsDragging)
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

	GridItemRef = nullptr;
	CanvasSlot = nullptr;
	
	// Remove bindings to allow mouse actions in gameplay when proxy is destroyed
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


