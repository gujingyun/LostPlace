// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentBase.generated.h"
class ULoadScreenSaveGame;
struct FLPGameplayTags;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& );
DECLARE_MULTICAST_DELEGATE(FAbilityGiven) //技能初始化应用后的回调委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&); //单播委托，只能绑定一个回调
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, int32 /*技能等级*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, const FGameplayTag& /*输入标签*/, const FGameplayTag& /*上一个输入标签*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*技能标签*/); //中止一个技能激活的回调
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivePassiveEffect, const FGameplayTag& /*被动技能标签*/, bool /*激活或取消*/); //被动技能特效监听委托，对应特效是否开启

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
	FAbilityEquipped AbilityEquipped; //技能装配更新回调
	FDeactivatePassiveAbility DeactivatePassiveAbility; //取消技能激活的委托
	FActivePassiveEffect ActivatePassiveEffect; //被动技能对应特效委托

	void AddCharacterAbilitiesFormSaveData(ULoadScreenSaveGame* SaveGameData);
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	
	bool bStartupAbilitiesGiven = false; //初始化应用技能后，此值将被设置为true，用于记录当前是否被初始化完成

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate); //遍历技能，并将技能广播出去

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(FGameplayAbilitySpec& AbilitySpec,const FGameplayTag& SlotTag); //技能是否有槽位
	static bool AbilityHasAnySlot(FGameplayAbilitySpec& AbilitySpec);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);

	/**
	 * 多网络被动特效委托广播，让每个客户端都可以看到特效
	 * @param AbilityTag 被动技能标签
	 * @param bActivate 激活或者关闭
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	
	void UpgradeAttribute(const FGameplayTag& AttributeTag); //升级属性

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag); //服务器升级属性函数

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag); //通过技能标签获取技能实例

	void UpdateAbilityStatuses(int32 Level); //根据角色等级更新技能数据状态

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag); //只在服务器端运行，消耗技能点函数提升技能等级


	
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Slot ); //只在服务器端运行，装配技能函数

	UFUNCTION(Client, Reliable) //在客户端处理技能装配
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription); //通过标签获取技能描述

	static void ClearSlot(FGameplayAbilitySpec* AbilitySpec);
	void ClearAbilitiesOfSlot(const FGameplayTag& SlotTag); //清空技能槽位的技能
	
protected:
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 AbilityLevel); //技能状态更新后回调
	
	virtual void OnRep_ActivateAbilities() override;

};
