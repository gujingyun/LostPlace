// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "LPPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPPassiveAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()
public:
	/**
	 * 覆写激活技能函数
	 * @param Handle 技能实力的句柄
	 * @param ActorInfo 技能拥有者
	 * @param ActivationInfo 激活信息
	 * @param TriggerEventData 游戏事件信息
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * 接收到技能结束回调函数
	 * @param AbilityTag 结束的技能标识标签
	 */
	void ReceiveDeactivate(const FGameplayTag& AbilityTag);

};
