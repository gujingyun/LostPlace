// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
/**
 * LPGameplayTags 标签 单例模式
 *
 * 内部包含原生的项目中使用的标签
 */
struct FLPGameplayTags
{
public:
	static const FLPGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();
	/*
 * Primary Attributes
 */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/*
	 * Secondary Attributes
	 */
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMP;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;


	//属性伤害抗性
	FGameplayTag Attributes_Resistance_Fire; //火属性伤害抵抗 标签
	FGameplayTag Attributes_Resistance_Lightning; //雷属性伤害抵抗 标签
	FGameplayTag Attributes_Resistance_Arcane; //魔法伤害抵抗 标签
	FGameplayTag Attributes_Resistance_Physical; //物理伤害抵抗 标签

	//元属性
    FGameplayTag Attributes_Meta_IncomingXP; //元属性经验 标签
	
	/*
	 *输入
	 */
	FGameplayTag InputTag_LMB; //鼠标左键
	FGameplayTag InputTag_RMB; //鼠标右键
	FGameplayTag InputTag_1; //1键
	FGameplayTag InputTag_2; //2键
	FGameplayTag InputTag_3; //3键
	FGameplayTag InputTag_4; //4键
	FGameplayTag InputTag_Passive_1; //被动技能1
	FGameplayTag InputTag_Passive_2; //被动技能2


	
	FGameplayTag Damage; //伤害 标签
	FGameplayTag Damage_Fire; //火属性伤害 标签
	FGameplayTag Damage_Lightning; //雷属性伤害 标签
	FGameplayTag Damage_Arcane; //魔法伤害 标签
	FGameplayTag Damage_Physical; //物理伤害 标签

	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack; //攻击
	FGameplayTag Abilities_Summon; //召唤技能激活标签

	FGameplayTag Abilities_HitReact; //受击技能标签

	FGameplayTag Abilities_Status_Locked; //技能状态 已锁定
	FGameplayTag Abilities_Status_Eligible; //技能状态 可解锁
	FGameplayTag Abilities_Status_Unlocked; //技能状态 已解锁
	FGameplayTag Abilities_Status_Equipped; //技能状态 已装配
	
	FGameplayTag Abilities_Type_Offensive; //技能类型 主动技能
	FGameplayTag Abilities_Type_Passive; //技能类型 被动技能
	FGameplayTag Abilities_Type_None; //技能类型 空 受击等技能设置

	
	FGameplayTag Abilities_Fire_FireBolt; //火球术技能标签
	FGameplayTag Abilities_Lightning_Electrocute; //雷击技能标签

	FGameplayTag Cooldown_Fire_FireBolt; //火球术冷却标签


	FGameplayTag CombatSocket_Weapon; //武器战斗插槽标签
	FGameplayTag CombatSocket_RightHand; //右手战斗插槽标签
	FGameplayTag CombatSocket_LeftHand; //左手战斗插槽标签
	FGameplayTag CombatSocket_Tail; //尾巴战斗插槽标签
	
	//使用攻击动作索引
	FGameplayTag Montage_Attack_1; 
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistance; //属性伤害标签对应属性抵抗标签

	FGameplayTag Effects_HitReact; //受击 标签
	



private:
	static FLPGameplayTags GameplayTags;
};

