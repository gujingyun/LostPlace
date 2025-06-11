// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/LPWidgetController.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/LPPlayerController.h"
#include "Player/LPPlayerState.h"

void ULPWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}
void ULPWidgetController::BroadcastInitialValues()
{
}

void ULPWidgetController::BindCallbacksToDependencies()
{
}

void ULPWidgetController::BroadcastAbilityInfo()
{
	if(!GetLPASC()->bStartupAbilitiesGiven) return; //判断当前技能初始化是否完成，触发回调时都已经完成

	//创建单播委托
	FForEachAbility BroadcastDelegate;
	//委托绑定回调匿名函数，委托广播时将会触发函数内部逻辑
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		//通过静态函数获取到技能实例的技能标签，并通过标签获取到技能数据
		FLPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAbilitySystemComponentBase::GetAbilityTagFromSpec(AbilitySpec));
		//获取到技能的输入标签
		Info.InputTag = UAbilitySystemComponentBase::GetInputTagFromSpec(AbilitySpec);
		//广播技能数据
		AbilityInfoDelegate.Broadcast(Info); 
	});
	//遍历技能并触发委托回调
	GetLPASC()->ForEachAbility(BroadcastDelegate);
}

ALPPlayerController* ULPWidgetController::GetLPPC()
{
	if (LPPlayerController == nullptr)
	{
		LPPlayerController = Cast<ALPPlayerController>(PlayerController);
	}
	return LPPlayerController;
}

ALPPlayerState* ULPWidgetController::GetLPPS()
{
	if (LPPlayerState == nullptr)
	{
		LPPlayerState = Cast<ALPPlayerState>(PlayerState);
	}
	return LPPlayerState;
}

UAbilitySystemComponentBase* ULPWidgetController::GetLPASC()
{
	if (AbilitySystemComponentBase == nullptr)
	{
		AbilitySystemComponentBase = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent);
	}
	return AbilitySystemComponentBase;
}

UAttributeSetBase* ULPWidgetController::GetLPAS()
{
	if (AttributeSetBase == nullptr)
	{
		AttributeSetBase = Cast<UAttributeSetBase>(AttributeSet);
	}
	return AttributeSetBase;
}
