// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category="输入")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 Level); //获取当前等级的技能描述
	virtual FString GetNextLevelDescription(int32 Level); //获取技能下一等级的技能描述
	static  FString GetLockedDescription(int32 Level); //获取锁定技能描述

protected:

	float GetManaCost(float InLevel = 1.f) const; //获取技能蓝量消耗
	float GetCooldown(float InLevel = 1.f) const; //获取技能冷却时间

};


