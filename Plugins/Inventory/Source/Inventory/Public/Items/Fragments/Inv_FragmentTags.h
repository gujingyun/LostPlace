#pragma once

#include "NativeGameplayTags.h"

namespace FragmentTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GridFragment) // 网格
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(IconFragment) // 图标
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StackableFragment) // 可堆叠
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ConsumableFragment) // 消耗品
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemNameFragment) // 物品名称
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(PrimaryStatFragment)		// 主属性
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentFragment)
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemTypeFragment) // 物品类型
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FlavorTextFragment) // 描述
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SellValueFragment) // 出售价值
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(RequiredLevelFragment)	// 要求等级
	// 状态修改器标签
	namespace StatMod
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatMod_1)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatMod_2)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatMod_3)
	}
}