// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/LPAbilitySystemGlobals.h"

#include "LPAbilityTypes.h"

FGameplayEffectContext* ULPAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FLPGameplayEffectContext();
}
