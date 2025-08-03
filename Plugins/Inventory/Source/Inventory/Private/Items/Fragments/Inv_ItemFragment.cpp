#include "Items/Fragments/Inv_ItemFragment.h"
#include "Engine/Engine.h"
#include "Components/SkeletalMeshComponent.h" 
#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "Player/Inv_PlayerController.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Image.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectUIData.h"

void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UInv_Leaf_Image* Image = Cast<UInv_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;

	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UInv_Leaf_Text* LeafText = Cast<UInv_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetText(FragmentText);
}
void FInv_LabeledNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	LabeledValue->SetText_Label(Text_Label, bCollapseLabel);

	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	
	LabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}

void FInv_LabeledNumberFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();

	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	bRandomizeOnManifest = false;
}

void FInv_ConsumeModifier::OnConsume(APlayerController* PC)
{
	ApplyGameplayEffectToTarget(PC);
	
	// 显示消耗品调试信息
	if (IsValid(GameplayEffectClass))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Cyan,
			FString::Printf(TEXT("Consumable effect applied: %s (Value: %f)"),
				*GameplayEffectClass->GetName(), GetValue()));
	}
}

void FInv_ConsumeModifier::ApplyGameplayEffectToTarget(APlayerController* PC) const
{
	if (!IsValid(GameplayEffectClass) || !IsValid(PC))
	{
		return;
	}
	
	// 获取玩家角色
	AActor* TargetActor = PC->GetPawn();
	if (!IsValid(TargetActor))
	{
		return;
	}
	
	// 获取AbilitySystemComponent
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))
	{
		return;
	}
	
	// 创建GameplayEffect上下文
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(TargetActor);
	
	// 创建GameplayEffect规格
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
	
	// 应用GameplayEffect
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void FInv_ConsumeModifier::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_LabeledNumberFragment::Assimilate(Composite);
	
	// 如果有GameplayEffect，尝试显示其描述
	if (IsValid(GameplayEffectClass))
	{
		FText EffectDescription = GetEffectDescription();
		if (!EffectDescription.IsEmpty())
		{
			// 更新标签为效果描述
			// 注意：这里可能需要根据实际的UI结构进行调整
		}
	}
}

FText FInv_ConsumeModifier::GetEffectDescription() const
{
	if (!IsValid(GameplayEffectClass))
	{
		return FText::GetEmpty();
	}
	
	// 获取GameplayEffect的默认对象来获取其属性
	if (const UGameplayEffect* DefaultEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>())
	{
		// 使用新的GameplayEffectComponent方式获取UI数据 (UE 5.3+)
		if (const class UGameplayEffectUIData* UIComponent = DefaultEffect->FindComponent<class UGameplayEffectUIData>())
		{
			// 从UIComponent获取描述信息
			// 注意：这里需要根据UGameplayEffectUIData的具体实现来获取描述文本
			// 如果UIComponent有Description属性，可以直接使用
			return FText::FromString("GameplayEffect UI Description"); // 这里需要根据实际UIComponent的属性调整
		}
		
		// 如果没有UI组件，尝试从效果名称生成描述
		FString EffectName = GameplayEffectClass->GetName();
		// 移除类名前缀（如"GE_"）
		if (EffectName.StartsWith(TEXT("GE_")))
		{
			EffectName = EffectName.RightChop(3);
		}
		return FText::FromString(EffectName);
	}
	
	return FText::GetEmpty();
}

void FInv_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnConsume(PC);
	}
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : ConsumeModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FInv_ConsumableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}


void FInv_EquipModifier::OnEquip(APlayerController* PC)
{
	ApplyGameplayEffectToTarget(PC);
	
	// 显示装备调试信息
	if (IsValid(GameplayEffectClass))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			FString::Printf(TEXT("Equipment effect applied: %s (Value: %f)"),
				*GameplayEffectClass->GetName(), GetValue()));
	}
}

void FInv_EquipModifier::OnUnequip(APlayerController* PC)
{
	RemoveGameplayEffectFromTarget(PC);
	
	// 显示卸载调试信息
	if (IsValid(GameplayEffectClass))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Red,
			FString::Printf(TEXT("Equipment effect removed: %s (Value: %f)"),
				*GameplayEffectClass->GetName(), GetValue()));
	}
}

void FInv_EquipModifier::ApplyGameplayEffectToTarget(APlayerController* PC)
{
	if (!IsValid(GameplayEffectClass) || !IsValid(PC))
	{
		return;
	}
	
	// 获取玩家角色
	AActor* TargetActor = PC->GetPawn();
	if (!IsValid(TargetActor))
	{
		return;
	}
	
	// 获取AbilitySystemComponent
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))
	{
		return;
	}
	
	// 创建GameplayEffect上下文
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(TargetActor);
	
	// 创建GameplayEffect规格
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
	
	// 应用GameplayEffect并保存句柄
	ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void FInv_EquipModifier::RemoveGameplayEffectFromTarget(APlayerController* PC)
{
	if (!IsValid(PC) || !ActiveEffectHandle.IsValid())
	{
		return;
	}
	
	// 获取玩家角色
	AActor* TargetActor = PC->GetPawn();
	if (!IsValid(TargetActor))
	{
		return;
	}
	
	// 获取AbilitySystemComponent
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))
	{
		return;
	}
	
	// 移除GameplayEffect
	TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
	ActiveEffectHandle = FActiveGameplayEffectHandle();
}


void FInv_EquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped) return;
	bEquipped = true;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnEquip(PC);
	}
}

void FInv_EquipmentFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped) return;
	bEquipped = false;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnUnequip(PC);
	}
}

void FInv_EquipmentFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : EquipModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FInv_EquipmentFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}
AInv_EquipActor* FInv_EquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	if (!IsValid(EquipActorClass) || !IsValid(AttachMesh)) return nullptr;

	AInv_EquipActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AInv_EquipActor>(EquipActorClass);
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);

	return SpawnedActor;
}

void FInv_EquipmentFragment::DestroyAttachedActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

void FInv_EquipmentFragment::SetEquippedActor(AInv_EquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}