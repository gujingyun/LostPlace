// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "LPFireBall.generated.h"
/**
 * 火焰爆发使用的火球类
 */
UCLASS()
class LOSTPLACE_API ALPFireBall : public AProjectile
{
	GENERATED_BODY()

public:

	ALPFireBall();
	//执行蓝图时间轴事件，需要在蓝图中实现此事件
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	UPROPERTY(BlueprintReadOnly) //当前火球返回的目标角色，默认是技能的释放者，在创建火球是创建
	TObjectPtr<AActor> ReturnToActor;
	
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
protected:
	virtual void BeginPlay() override;
	virtual  void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnHit() override;

};
