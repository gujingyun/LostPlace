// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoints/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Core/LPGameMode.h"
#include "Interface/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::LoadActor_Implementation()
{
	// 加载地图入口时不执行任何操作
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if(OtherActor->ActorHasTag("Player")) //如果只需要判断是不是玩家角色通过标签判断即可
	if(OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		if(ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			//保存场景状态
			LPGameMode->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		
		//修改存档当的检测点
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
