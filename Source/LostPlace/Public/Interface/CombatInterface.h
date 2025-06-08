// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAnimMontage;
class UNiagaraSystem;
//蒙太奇动画和标签以及骨骼位置的映射，用于攻击技能获取和设置攻击范围
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	//使用的蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	//对应的标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	//部位对应的标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;
	
	//攻击时的触发伤害的骨骼插槽
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName CombatTipSocketName; //设置技能释放的位置

	//攻击时的触发音效
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOSTPLACE_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel(); //获取玩家等级

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag CombatTag);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const; //获取当前角色是否死亡

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar(); //获取当前角色

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect(); //获取角色的受伤特效

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag); //通过标签获取对应的结构体

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount(); //获取角色拥有的仆从数量

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncrementMinionCount(const int32 Amount); //设置角色仆从的增长数量，负数为负增长

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass(); //获取当前角色的职业

};
