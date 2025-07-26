// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"
#include "Engine/Texture2D.h"  // 添加UTexture2D的头文件引用

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"

void UInv_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInv_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (IsValid(EquippedSlottedItem)) return;
	
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnoccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UInv_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}

UInv_EquippedSlottedItem* UInv_EquippedGridSlot::OnItemEquipped(UInv_InventoryItem* Item,
	const FGameplayTag& EquipmentTag, float TileSize)
{
	// 检查装备类型标签
	if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;
	// 获取网格维度
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item,FragmentTags::GridFragment);
	if (!GridFragment) return nullptr;
	const FIntPoint GridDimensions = GridFragment->GetGridSize();
	
	// 计算所装备的槽位物品的绘制大小
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D DrawSize = GridDimensions * IconTileWidth;
	
	// 创建 Equipped Slotted Item 小部件
	EquippedSlottedItem = CreateWidget<UInv_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);
	
	// 设置槽位物品的 Inventory Item
	EquippedSlottedItem->SetInventoryItem(Item);
	
	// 设置槽位物品的 Equipment Type 标签
	EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTypeTag);
	
	// 隐藏 Slotted Item 上的 Stack Count 小部件
	EquippedSlottedItem->UpdateStackCount(0);
	
	// 在此类上设置库存物品（装备的网格槽位）
	SetInventoryItem(Item);
	
	// 在装备的槽状物品上设置 Image Brush
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item,FragmentTags::IconFragment);
	if (!ImageFragment) return nullptr;

	FSlateBrush Brush;
	Brush.SetResourceObject(Cast<UObject>(ImageFragment->GetIcon()));  // 添加显式类型转换
	Brush.ImageSize = DrawSize;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	EquippedSlottedItem->SetImageBrush(Brush);
	
	// 将 Slotted Item 作为子项添加到此小部件的 Overlay 中
	Overlay_Root->AddChildToOverlay(EquippedSlottedItem);
	FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
	auto OverlaySize = OverlayGeometry.Size;
	auto OverlayPos = OverlayGeometry.Position;
	const float LeftPadding = OverlaySize.X / 2.0f - DrawSize.X / 2.0f;
	const float TopPadding = OverlaySize.Y / 2.0f - DrawSize.Y / 2.0f;
	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));
	
	// 返回 Equipped Slotted Item 小部件
	return EquippedSlottedItem;
}
