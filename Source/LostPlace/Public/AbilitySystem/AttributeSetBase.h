// Fill out your copyright notice in the Description page of Project Settings.

// 版权归暮志未晚所有。

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY();

	FEffectProperties(){};

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	
	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;
	
	UPROPERTY()
	AController* TargetController = nullptr;
	
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	
};

// typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFuncPtr = typename  TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */
UCLASS()
class LOSTPLACE_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAttributeSetBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual  void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
	// 定义一个默认的委托用户策略
	// TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FunctionPpinter;
	
	//重要属性
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Health, Category="重要属性")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health);
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MP, Category="重要属性")
	FGameplayAttributeData MP;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MP);
	/*
	 * 基础属性
	 * Strength:力量
	 *					物理攻击
	 *					物理攻击范围
	 *					护甲穿透
	 * Intelligence:智力
	 * 					魔法攻击
	 * 					最大魔法值
	 * 					魔法攻击范围
	 * 					魔法恢复
	 * 					魔法穿透
	 * Resilience:韧性
	 * 					物理防御
	 * 					魔法防御
	 * 					物理抗性
	 * 					魔法抗性
	 * Vigor:活力
	 * 					最大生命值
	 * 					生命值恢复
	 * Agile:敏捷
	 * 					移动速度
	 * 					攻击速度
	 * 					交互速度
	 * concentrate:专注
	 * 					施法速度
	 * 					技能CD
	 * 					暴击率
	 * 					暴击伤害
	 */
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Strength, Category="基础属性")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Strength);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Intelligence, Category="基础属性")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Intelligence);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Resilience, Category="基础属性")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Resilience);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Vigor, Category="基础属性")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Vigor);


	
	/*
	 * 副属性
	 */
	// MaxHealth 血量上限，基于Vigor 体力属性计算
	// MaxMana 蓝量上限，基于Intelligence 智力属性
	// Armor 防御，基于Resilience 韧性属性计算， 降低所受伤害
	// ArmorPenetration 护甲穿透，基于Resilience 韧性属性计算，降低敌人的防御，增加暴击率
	// BlockChance 格挡率 ，基于Armor 防御属性计算，增加格挡伤害概率，触发时，降低一半所受伤害
	// CriticalHitChance 暴击率，基于ArmorPenetration 护甲穿透属性计算，增加触发暴击伤害的概率
	// CriticalHitDamage 暴击伤害，基于ArmorPenetration 护甲穿透属性计算，触发暴击时基于增加的伤害量
	// CriticalHitResistance 暴击抵抗，基于Armor 防御属性计算，降低敌人的暴击概率
	// HealthRegeneration 血量自动恢复，基于Vigor 体力属性计算，每秒自动恢复一定血量
	// ManaRegeneration 蓝量自动恢复，基于Intelligence 智力属性，每秒自动恢复蓝量



	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxHealth, Category="副属性")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxHealth);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMP, Category="副属性")
	FGameplayAttributeData MaxMP;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxMP);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Armor, Category="副属性")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Armor);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ArmorPenetration, Category="副属性")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_BlockChance, Category="副属性")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, BlockChance);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitChance, Category="副属性")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, CriticalHitChance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitDamage, Category="副属性")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitResistance, Category="副属性")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, CriticalHitResistance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_HealthRegeneration, Category="副属性")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, HealthRegeneration);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ManaRegeneration, Category="副属性")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, ManaRegeneration);


	/*
	 * 属性伤害抗性
	*/

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category="抗性属性")
	FGameplayAttributeData FireResistance; // 火属性抗性
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, FireResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category="抗性属性")
	FGameplayAttributeData LightningResistance; // 雷属性抗性
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, LightningResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category="抗性属性")
	FGameplayAttributeData ArcaneResistance; // 魔法抗性
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, ArcaneResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category="抗性属性")
	FGameplayAttributeData PhysicalResistance; // 物理抗性
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, PhysicalResistance);


	/*
	 * 元属性
	 * IncomingDamage 传入的伤害，用于处理伤害减免
	 */
	UPROPERTY(BlueprintReadOnly, Category="元属性")
	FGameplayAttributeData IncomingDamage; //处理传入的伤害
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category="元属性")
	FGameplayAttributeData IncomingXP; //处理传入的经验
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, IncomingXP);





	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;


	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_MP(const FGameplayAttributeData& OldMP) const;
	
	UFUNCTION()
	void OnRep_MaxMP(const FGameplayAttributeData& OldMaxMP) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectProperties& Props) const;
	
	FORCEINLINE_DEBUGGABLE void ShowFloatingText(const FEffectProperties& Props, float Damage,bool bBlockedHit,bool bCriticalHit) const;

	//发送经验事件
	void SendXPEvent(const FEffectProperties& Props);

	 //将血量和蓝量填充满, 我们将设置变量
	bool bFillHealth = true;
	bool bFillMana = true;
  
};
