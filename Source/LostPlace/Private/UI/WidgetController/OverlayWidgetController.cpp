// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "LPGameplayTags.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeSetBase.h"

#include "LostPlace/LPLogChannels.h"
#include "Player/LPPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetLPAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetLPAS()->GetMaxHealth());
	OnMPChanged.Broadcast(GetLPAS()->GetMP());
	OnMaxMPChanged.Broadcast(GetLPAS()->GetMaxMP());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{

	GetLPPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);

	//绑定等级相关回调
	GetLPPS()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChangeDelegate.Broadcast(NewLevel);
	});

	

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetLPAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetLPAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetLPAS()->GetMPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMPChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetLPAS()->GetMaxMPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxMPChanged.Broadcast(Data.NewValue);
		}
	);
	if (GetLPASC())
	{

		
		GetLPASC()->AbilityEquipped.AddUObject(this, &ThisClass::OnAbilityEquipped);
		if(GetLPASC()->bStartupAbilitiesGiven)
		{
			//如果执行到此处时，技能的初始化工作已经完成，则直接调用初始化回调
			BroadcastAbilityInfo();
		}
		else
		{
			//如果执行到此处，技能初始化还未完成，将通过绑定委托，监听广播的形式触发初始化完成回调
			GetLPASC()->AbilityGivenDelegate.AddUObject(this, &ThisClass::BroadcastAbilityInfo);
		}
		GetLPASC()->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& TagsContainer)
		{
			for (const FGameplayTag& Tag : TagsContainer)
			{
					
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});
	}
	

	
}


void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetLPPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("无法查询到等级相关数据，请查看PlayerState是否设置了对应的数据"));
	const int32 Level =  LevelUpInfo->FindLevelForXP(NewXP); //根据当前经验值查询对应的等级
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num(); //获取当前最大等级
	if(Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement; //获取对应等级的升级经验值
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;
		const float XPPercent = static_cast<float>(NewXP - PreviousLevelUpRequirement) / (LevelUpRequirement - PreviousLevelUpRequirement);
		OnXPPercentChangedDelegate.Broadcast(XPPercent);
	}
	
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{

	const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();

	//清除旧插槽的数据
	FLPAbilityInfo  LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	//更新新插槽的数据
	FLPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	
}




