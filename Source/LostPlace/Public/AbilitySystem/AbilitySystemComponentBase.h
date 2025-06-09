// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& );
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityGiven, UAbilitySystemComponentBase*) //技能初始化应用后的回调委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&); //单播委托，只能绑定一个回调

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
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	
	bool bStartupAbilitiesGiven = false; //初始化应用技能后，此值将被设置为true，用于记录当前是否被初始化完成

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate); //遍历技能，并将技能广播出去

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	void UpgradeAttribute(const FGameplayTag& AttributeTag); //升级属性

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag); //服务器升级属性函数

protected:
	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	virtual void OnRep_ActivateAbilities() override;

};
