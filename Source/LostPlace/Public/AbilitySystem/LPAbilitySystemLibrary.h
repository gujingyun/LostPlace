// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LPAbilitySystemLibrary.generated.h"
class ULootTiers;
class ULoadScreenSaveGame;
struct FDamageEffectParams;
struct FWidgetControllerParams;
class USpellMenuWidgetController;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class ALPHUD;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/*
	 * 创建OverlayWidgetController的参数
	 */
	
	UFUNCTION(BlueprintPure, Category = "LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static bool MakeOverlayWidgetControllerPrParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams,ALPHUD*& OutHUD);
	
	UFUNCTION(BlueprintPure, Category = "LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	/*
	 * 能力系统类默认值
	 */
	
	//初始化角色的属性
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	/**
	 * 从存档初始化角色的属性
	 *
	 * @param WorldContextObject 一个世界场景的对象，用于获取当前所在的世界
	 * @param ASC 角色的技能系统组件
	 * @param SaveGame 角色使用的存档指针
	 *
	 * @return void 
	 *
	 * @note 这个函数主要用于从存档里读取角色信息，并初始化
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf = "WorldContextObject"))
	static void InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame);

	
	//初始化角色的技能
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);
	
	//获取根据敌人类型和等级获取敌人产生的经验
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);

	/**
	 * 获取生成的战利品数据资产，此数据会配置到GameMode上
	 * @param WorldContextObject  一个世界场景的对象，用于获取当前所在的世界
	 * @return 战利品数据
	 *
	 * @note 敌人死亡后，所需生成的战利品
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf = "WorldContextObject"))
	static ULootTiers* GetLootTiers(const UObject* WorldContextObject);

	/*
	 * 影响上下文的获取器和设置器
	 */
	
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsCriticalHit);

	//获取当前GE是否成功应用负面效果
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果伤害
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static float GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果持续时间
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static float GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果触发间隔
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static float GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	
	//获取当前GE负面效果伤害类型
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);


	//设置GE是否应用负面效果
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDeBuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDeBuff);

	//设置GE负面效果相关数值 负面效果伤害类型 负面效果伤害 负面效果持续时间 负面效果触发间隔时间
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetDeBuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, FGameplayTag& InDamageType, float InDamage, float InDuration, float InFrequency);

	//获取当前GE死亡冲击的方向和力度
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	
	//设置GE是否触发暴击
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& bInDeathImpulse);
	
	//获取击退的方向和力度
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);
	
	//设置GE击退的方向和力度
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce);
	/**
	 * 获取当前GE是否为范围伤害GE
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 *
	 * @return 如果是范围伤害 返回true
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 获取当前GE 范围伤害内半径
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 *
	 * @return 返回负面效果触发间隔
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 获取当前GE 范围伤害外半径
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 *
	 * @return 返回负面效果触发间隔
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 获取当前GE 伤害中心点
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 *
	 * @return 攻击的击退会根据概率计算，如果有值，则为应用成功
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);
	/**
	 * 设置GE是否为范围伤害
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 * @param bInIsRadialDamage true为设置为范围伤害
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);

	/**
	 * 设置GE 范围伤害 内半径距离
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 * @param InRadialDamageInnerRadius 内半径距离 内半径内受到完整伤害
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InRadialDamageInnerRadius);

	/**
	 * 设置GE 范围伤害 外半径距离
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 * @param InRadialDamageOuterRadius 外半径距离，超出此距离外的敌人将无法受到伤害
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InRadialDamageOuterRadius);
	
	/**
	 * 设置GE伤害源的中心点
	 *
	 * @param EffectContextHandle 当前GE的上下文句柄
	 * @param InRadialDamageOrigin 伤害源的中心点
	 *
	 * @note 此属性是LPAbilityTypes.h内自定义属性，可实现复制。
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InRadialDamageOrigin);



	/*
	 * 游戏机制相关函数
	 */
	
	//获取到攻击位置半径内的所有动态Actor
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	//获取双方是否为敌对势力
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);


	//通过技能生成的负面配置项应用技能负面效果
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	/**
	 * 这个函数根据传入的值计算均匀分布的多段角度，
	 *
	 * @param Forward 正前方向
	 * @param Axis 基于旋转的轴
	 * @param Spread 角度范围
	 * @param NumRotators 分段数
	 *
	 * @return TArray<FRotator&> 返回每段角度的中间角度的数组
	 *
	 * @note 这个函数用于在技能生成投掷物的函数逻辑中。
	 */
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector & Axis, float Spread, int32 NumRotators);

	/**
	 * 这个函数根据传入的值计算均匀分布的多段朝向
	 *
	 * @param Forward 正前方向
	 * @param Axis 基于旋转的轴
	 * @param Spread 角度范围
	 * @param NumVectors 分段数
	 *
	 * @return TArray<FVector&> 返回每段角度的中间角度的朝向数组
	 *
	 * @note 这个函数用于在技能生成投掷物的函数逻辑中。
	 */
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector & Axis, float Spread, int32 NumVectors);
	/**
	 * 获取距离目标位置最近的几个目标
	 * @param MaxTargets 获取最大目标的数量
	 * @param Actors 需要计算的目标数组
	 * @param OutClosestTargets 返回获取到的最近的目标
	 * @param Origin 计算的位置
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);

	/*
	 * 伤害效果参数
	 */
	
	/**
	 * 修改伤害配置项，将其设置为具有范围伤害的配置项
	 * @param DamageEffectParams 需要修改的配置项
	 * @param bIsRadial 设置是否为范围伤害
	 * @param InnerRadius 内半径
	 * @param OutRadius 外半径
	 * @param Origin 伤害中心
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamageEffectParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OutRadius, FVector Origin);

	/**
	 * 修改伤害的冲击力的方向
	 * @param DamageEffectParams 需要修改的伤害配置项
	 * @param KnockbackDirection 攻击时触发击退的方向
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude = 0.f);
	
	/**
	 * 修改伤害的冲击力的方向
	 * @param DamageEffectParams 需要修改的伤害配置项
	 * @param ImpulseDirection 死亡时触发击退的方向
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulseDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude = 0.f);

	/**
	 * 设置伤害配置应用目标ASC
	 * @param DamageEffectParams 需要修改的伤害配置 
	 * @param InASC 应用目标ASC
	 */
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetEffectParamsTargetASC(UPARAM(ref) FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC);

};
