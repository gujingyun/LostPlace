// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"

void ULPDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	// FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.0f);
	// const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaleDamage);
	//
	// //将GE应用给目标
	// GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
	// 	*DamageSpecHandle.Data.Get(),
	// 	UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));

	//生成配置
	FDamageEffectParams Params = MakeDamageEffectParamsFromClassDefaults(TargetActor);

	//通过配置项应用给目标ASC
	ULPAbilitySystemLibrary::ApplyDamageEffect(Params);
}

FDamageEffectParams ULPDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	// Params.DeBuffDamageType = DeBuffDamageType;
	Params.DeBuffChance = DeBuffChance;
	Params.DeBuffDamage = DeBuffDamage;
	Params.DeBuffDuration = DeBuffDuration;
	Params.DeBuffFrequency = DeBuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	//设置死亡冲击和击退
	if(IsValid(TargetActor))
	{
		//获取到攻击对象和目标的朝向，并转换成角度
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f; //设置击退角度垂直45度
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		//判断攻击是否触发击退
		if(FMath::RandRange(1, 100) < Params.KnockbackChance)
		{
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
	}
	return Params;
}

FTaggedMontage ULPDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages)
{
	if(TaggedMontages.Num() > 0)
	{
		const int32 RandomNum = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[RandomNum];
	}
	return FTaggedMontage();
}
