// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPGameplayTags.h"
#include "LPWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FLPGameplayTags;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsEnabled, bool,bEquipEnabled, FString, SpellDescription, FString, SpellNextLevelDescription);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag& ,AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag& ,AbilityTag);
//在技能面板选中的技能的标签结构体
struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag(); //技能标签
	FGameplayTag Status = FGameplayTag(); //技能状态标签
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LOSTPLACE_API USpellMenuWidgetController : public ULPWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override; //广播初始化的值
	virtual void BindCallbacksToDependencies() override; //绑定数值变动后回调的广播
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStateChangedSignature SpellPointsChangedDelegate; //监听属性点的变化委托
	
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate; //选中技能按钮后，升级和装备按钮的变动回调
	
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag); //技能按钮选中调用函数，处理升级按钮和装配

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed(); //升级按钮调用函数

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect(); //取消按钮选中处理

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed(); //装配按钮调用函数

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag,const FGameplayTag& AbilityType);

	//监听技能装配后的处理
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

 
private:

	//通过技能状态标签和可分配技能点数来获取技能是否可以装配和技能是否可以升级
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus,int32 SpellPoints, bool& bShouldEnableSpellPoints, bool& bShouldEnableEquipButton);
	//设置选中技能默认值
	FSelectedAbility SelectedAbility = {FLPGameplayTags::Get().Abilities_None, FLPGameplayTags::Get().Abilities_Status_Locked};
	//保存当前技能可分配点数
	int32 CurrentSpellPoints = 0;

	bool bWaitingForEquipSelection = false;

	FGameplayTag SelectedSlot;
};
