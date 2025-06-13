// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void ULPDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.0f);
	for (auto Pair : DamageTypes)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaleDamage);
	}
	//将GE应用给目标
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
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

float ULPDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DGameplayTag)
{
	checkf(DamageTypes.Contains(DGameplayTag), TEXT("DamageTypes 未包含指定的 DamageType"),*GetNameSafe(this),*DGameplayTag.ToString());
	return DamageTypes[DGameplayTag].GetValueAtLevel(InLevel);
}
