// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
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
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	

	UPROPERTY(EditDefaultsOnly,  Category="伤害")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	//从角色设置的蒙太奇数组总，随机一个蒙太奇使用
	UFUNCTION(BlueprintPure)
	static FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages);

	float GetDamageByDamageType(float InLevel,const FGameplayTag& DGameplayTag);
};
