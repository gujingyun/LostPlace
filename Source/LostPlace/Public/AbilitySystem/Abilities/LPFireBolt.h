// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPProjectileSpell.h"
#include "LPFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPFireBolt : public ULPProjectileSpell
{
	GENERATED_BODY()
public:
		
	virtual FString GetDescription(int32 Level) override; //获取投射技能描述
	virtual FString GetNextLevelDescription(int32 Level) override; //获取投射技能下一等级描述
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, const bool bOverridePitch = false, const float PitchOverride = 0.f, AActor* HomingTarget = nullptr);

protected:

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float ProjectileSpread = 90.f; //攻击角度

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	int32 MaxNumProjectiles = 5; //最大生成火球数量

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMin = 1600.f; //移动朝向目标的最小加速度

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float HomingAccelerationMax = 3200.f; //移动朝向目标的最大加速度

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	bool bLaunchHomingProjectiles = true; //设置生成的飞弹是否需要朝向目标飞行

};
