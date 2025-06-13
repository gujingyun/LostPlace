// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& );
DECLARE_MULTICAST_DELEGATE(FAbilityGiven) //技能初始化应用后的回调委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&); //单播委托，只能绑定一个回调
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, int32 /*技能等级*/);

/**
 * 
 */
UCLASS()
class LOSTPLACE_API UAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();
	FEffectAssetTags EffectAssetTags;
	FAbilityGiven AbilityGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged; //技能状态更新委托

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	
	bool bStartupAbilitiesGiven = false; //初始化应用技能后，此值将被设置为true，用于记录当前是否被初始化完成

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate); //遍历技能，并将技能广播出去

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag); //升级属性

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag); //服务器升级属性函数

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag); //通过技能标签获取技能实例

	void UpdateAbilityStatuses(int32 Level); //根据角色等级更新技能数据状态

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag); //只在服务器端运行，消耗技能点函数提升技能等级

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription); //通过标签获取技能描述

protected:
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 AbilityLevel); //技能状态更新后回调
	
	virtual void OnRep_ActivateAbilities() override;

};
