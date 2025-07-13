// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckPoint.h"
#include "MapEntrance.generated.h"

UCLASS()
class LOSTPLACE_API AMapEntrance : public ACheckPoint
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	explicit  AMapEntrance(const FObjectInitializer& ObjectInitializer);

	/*   Save Interface   */
	virtual void LoadActor_Implementation() override; //通过存档二进制修改Actor数据后，更新Actor
	/*   End Save Interface   */
	
	/*   高亮接口   */
	virtual void HighlightActor_Implementation() override; //高亮
	/*  高亮接口结束   */

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap; //目标世界
	
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag; //目标玩家出生点
	
protected:


	/**
	 * 球碰撞体和物体发生碰撞后的回调
	 * @param OverlappedComponent 发生重叠事件的自身的碰撞体对象
	 * @param OtherActor 目标的actor对象
	 * @param OtherComp 目标的碰撞体组件
	 * @param OtherBodyIndex 目标身体的索引
	 * @param bFromSweep 是否为瞬移检测到的碰撞
	 * @param SweepResult 如果位置发生过瞬移（直接设置到某处），两个位置中间的内容会记录到此对象内
	 */
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
