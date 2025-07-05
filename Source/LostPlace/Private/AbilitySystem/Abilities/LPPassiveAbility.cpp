// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPPassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"

void ULPPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//获取到ASC
	if(UAbilitySystemComponentBase* LPASC = Cast<UAbilitySystemComponentBase>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		//绑定技能取消回调
		LPASC->DeactivatePassiveAbility.AddUObject(this, &ULPPassiveAbility::ReceiveDeactivate);
	}
}

void ULPPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	//判断技能标签容器里是否包含此标签
	if(AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
