// Fill out your copyright notice in the Description page of Project Settings.


#include "LPGameplayTags.h"

#include "GameplayTagsManager.h"

FLPGameplayTags FLPGameplayTags::GameplayTags;

void FLPGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Primary Attributes
	 */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage")
		);
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage")
		);
	
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increases Armor and Armor Penetration")
		);
	
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increases Health")
		);
	
	/*
	 * Secondary Attributes
	 */
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum amount of Health obtainable")
		);
	
	GameplayTags.Attributes_Secondary_MaxMP = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMP"),
		FString("Maximum amount of Mana obtainable")
		);
	
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken, improves Block Chance")
		);
		
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignored Percentage of enemy Armor, increases Critical Hit Chance")
		);
	
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit is scored")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduces Critical Hit Chance of attacking Enemies")
		);
	
	
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every 1 second")
		);
	
	
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every 1 second")
		);

	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("鼠标左键")
		);
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("鼠标右键")
		);
	
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("键盘1键")
		);
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("键盘2键")
		);
	
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("键盘3键")
		);
	
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("键盘4键")
		);
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("InputTag.Passive.1"),
			FString("被动技能1")
			);
	
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("InputTag.Passive.2"),
			FString("被动技能2")
		);


	
	GameplayTags.Damage = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage"),
			FString("伤害标签")
			);
	
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Fire"),
			FString("火属性伤害")
		);
	
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Lightning"),
			FString("雷属性伤害")
		);
	
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Arcane"),
			FString("魔法伤害")
		);
	
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Physical"),
			FString("物理伤害")
		);

	
	/* 属性抗性标签 */
	
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Fire"),
			FString("火属性抗性")
			);
	
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Lightning"),
			FString("雷属性抗性")
			);
	
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Arcane"),
			FString("魔法伤害抗性")
			);
	
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Physical"),
			FString("物理伤害抗性")
		);

	/**
	 * 元属性
	 */
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Attributes.Meta.IncomingXP"),
			FString("经验元属性标签")
			);
	
	/* 将属性和抗性标签对应 */
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/* 效果标签 */
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Effects.HitReact"),
			FString("受到攻击时，赋予的标签")
		);
	

	/* 能力标签 */
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Attack"),
			FString("攻击能力标签")
		);
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("召唤技能标签")
		);
	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Fire.FireBolt"),
		FString("火球术技能标签")
		);

	/**
	 * 技能冷却标签
	 */
	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Cooldown.Fire.FireBolt"),
			FString("火球术冷却标签")
			);


	/*
	* Montage
	*/
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("CombatSocket.Weapon"),
			FString("武器战斗插槽标签")
			);
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("CombatSocket.LeftHand"),
			FString("左手战斗插槽标签")
			);
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("CombatSocket.RightHand"),
			FString("右手战斗插槽标签")
		);
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("CombatSocket.Tail"),
		FString("尾巴战斗插槽标签")
	);


	/*
	* Montage Attack Index
	*/
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("第1个蒙太奇攻击标签")
		);

	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.2"),
			FString("第2个蒙太奇攻击标签")
			);

	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.3"),
			FString("第3个蒙太奇攻击标签")
			);

	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.4"),
			FString("第4个蒙太奇攻击标签")
			);




}