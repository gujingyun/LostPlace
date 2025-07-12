// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoints/CheckPoint.h"

#include "Components/SphereComponent.h"
#include "Core/LPGameMode.h"
#include "Interface/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


ACheckPoint::ACheckPoint(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	//关闭帧更新
	PrimaryActorTick.bCanEverTick = false;

	//创建检测点显示模型
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //设置查询并产生物理
	CheckpointMesh->SetCollisionResponseToChannels(ECR_Block); //设置阻挡所有物体与其重叠

	//设置球碰撞体
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); //设置其忽略所有碰撞检测
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //设置其与Pawn类型物体产生重叠事件
}

void ACheckPoint::LoadActor_Implementation()
{
	if(bReached)
	{
		HandleGlowEffects();
	}
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();

	//绑定重叠事件
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnSphereOverlap);
}

void ACheckPoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if(OtherActor->ActorHasTag("Player")) //如果只需要判断是不是玩家角色通过标签判断即可
	if(OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		if(ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			//保存场景状态
			LPGameMode->SaveWorldState(GetWorld());
		}
		//修改存档当的检测点
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		
		//如果与碰撞体重叠的是
		HandleGlowEffects();
	}
}
void ACheckPoint::HandleGlowEffects()
{
	//取消碰撞检查
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//创建一个新材质实例，修改效果
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance); //触发检查点修改材质后的回调
}
