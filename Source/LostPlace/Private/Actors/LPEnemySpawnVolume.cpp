// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LPEnemySpawnVolume.h"

#include "Actors/LPEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Interface/PlayerInterface.h"


// Sets default values
ALPEnemySpawnVolume::ALPEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //仅用于坚持
	Box->SetCollisionObjectType(ECC_WorldStatic); //设置物体类型
	Box->SetCollisionResponseToChannels(ECR_Ignore); //忽略所有检测通道
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //开启与pawn的重叠检测
}

void ALPEnemySpawnVolume::LoadActor_Implementation()
{
	if(bReached)
	{
		Destroy();
	}
}

void ALPEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxOverlap);
}

void ALPEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->Implements<UPlayerInterface>()) return;
	
	bReached = true;

	//在设置的所有点位生成敌人
	for(ALPEnemySpawnPoint* SpawnPoint : SpawnPoints)
	{
		if(IsValid(SpawnPoint))
		{
			SpawnPoint->SpawnEnemy();
		}
	}

	//设置不在产生物理查询，直接销毁无法保存到存档
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

