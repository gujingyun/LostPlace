// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "LPAbilityTypes.h"
#include "Interface/CombatInterface.h"
#include "LPDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPDamageGameplayAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	//创建技能负面效果使用的结构体
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector KnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	
	UPROPERTY(EditDefaultsOnly,  Category="伤害")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly,  Category="伤害")
	FScalableFloat Damage;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="伤害")
	// FGameplayTag DeBuffDamageType = FGameplayTag(); //负面效果伤害类型
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="伤害")
	float DeBuffChance = 20.f; //触发负面的机率
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="伤害")
	float DeBuffDamage = 5.f; //负面伤害

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="伤害")
	float DeBuffFrequency = 1.f; //负面伤害触发间隔时间

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="伤害")
	float DeBuffDuration = 5.f; //负面效果持续时间

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="伤害")
	float DeathImpulseMagnitude = 1000.f; //死亡时受到的冲击力


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float KnockbackForceMagnitude = 1000.f; //技能击中敌人后，敌人受到的击退的力度

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float KnockbackChance = 0.f; //技能命中敌人触发击退的概率

	//当前伤害类型是否为范围伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	bool bIsRadialDamage = false;

	//内半径：在此半径内的所有目标都将受到完整的伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageInnerRadius = 0.f;

	//外半径：超过这个距离的目标受到最小伤害，最小伤害如果设置为0，则圈外不受到伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageOuterRadius = 0.f;

	//伤害源的中心点
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FVector RadialDamageOrigin = FVector::ZeroVector;
	
	//从角色设置的蒙太奇数组总，随机一个蒙太奇使用
	UFUNCTION(BlueprintPure)
	static FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages);



};
