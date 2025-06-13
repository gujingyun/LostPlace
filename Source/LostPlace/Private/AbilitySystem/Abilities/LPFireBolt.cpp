// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPFireBolt.h"

#include "LPGameplayTags.h"

FString ULPFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level,FLPGameplayTags::Get().Damage_Fire); //根据等级获取技能伤害
	const float MPCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
	// 标题
	"<Title>火球术</>\n"

	// 细节
	"<Small>等级：</> <Level>%i</>\n"
	"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
	"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

	// 技能描述
	"<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),

	// 动态修改值
	Level,
	Cooldown,
	MPCost,
	FMath::Min(Level, NumProjectiles),
	Damage);
}

FString ULPFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level,FLPGameplayTags::Get().Damage_Fire); //根据等级获取技能伤害
	const float MPCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
			// 标题
			"<Title>下一等级</>\n"

			// 细节
			"<Small>等级：</> <Level>%i</>\n"
			"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
			"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

			// 技能描述
			"<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),

			// 动态修改值
			Level,
			Cooldown,
			MPCost,
			FMath::Min(Level, NumProjectiles),
			Damage);
}
