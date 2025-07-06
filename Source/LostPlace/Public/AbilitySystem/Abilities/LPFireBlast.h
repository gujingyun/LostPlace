// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPDamageGameplayAbility.h"
#include "LPFireBlast.generated.h"

class ALPFireBall;
/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPFireBlast : public ULPDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override; //获取投射技能描述
	virtual FString GetDescriptionAtLevel(int32 Level, const FString& Title) override;
	virtual FString GetNextLevelDescription(int32 Level) override; //获取投射技能下一等级描述

	/**
	 * 生成技能所需的火球
	 * @return NumFireBalls个数火球的数组
	 */
	UFUNCTION(BlueprintCallable)
	TArray<ALPFireBall*> SpawnFireBalls();

protected:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="FireBlast")
	int32 NumFireBalls = 12;
private:

	UPROPERTY(EditDefaultsOnly, Category="FireBlast") //生成火球使用的类
	TSubclassOf<ALPFireBall> FireBallClass;
};
