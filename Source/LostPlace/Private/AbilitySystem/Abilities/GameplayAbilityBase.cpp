// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbilityBase.h"

#include "AbilitySystem/AttributeSetBase.h"

FString UGameplayAbilityBase::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s,</> <Level>%d</>"), L"默认技能名称 - 火球术火球术火球术火球术", Level);
}

FString UGameplayAbilityBase::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>下一等级：</><Level>%d</> 造成更多的伤害。"), Level);
}

FString UGameplayAbilityBase::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>技能将在角色等级达到</><Level>%d</>时解锁"), Level);
}
float UGameplayAbilityBase::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	//获取到冷却GE
	if(const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		//遍历GE修改的内容
		for(FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			//判断修改的属性是否为角色蓝量属性
			if(Mod.Attribute == UAttributeSetBase::GetMPAttribute())
			{
				//通过修饰符获取到使用的FScalableFloat并计算传入等级的蓝量消耗，FScalableFloat是受保护性的属性，无法直接获取，只能通过函数
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break; //获取到了就结束遍历
			}
		}
	}
	return ManaCost;
}

float UGameplayAbilityBase::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	//获取到技能冷却GE
	if(const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		//获取到当前冷却时间
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}