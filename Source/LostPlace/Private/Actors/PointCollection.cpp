// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PointCollection.h"

#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"


APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	//设置Pt_0作为根节点
	Pt_0 = CreateDefaultSubobject<USceneComponent>("Pt_0");
	ImmutablePts.Add(Pt_0);
	SetRootComponent(Pt_0);

	//将其它的场景组件作为根组件的子节点添加，会在蓝图中去修改它的位置
	CreateSceneComponent(Pt_1, "Pt_1");
	CreateSceneComponent(Pt_2, "Pt_2");
	CreateSceneComponent(Pt_3, "Pt_3");
	CreateSceneComponent(Pt_4, "Pt_4");
	CreateSceneComponent(Pt_5, "Pt_5");
	CreateSceneComponent(Pt_6, "Pt_6");
	CreateSceneComponent(Pt_7, "Pt_7");
	CreateSceneComponent(Pt_8, "Pt_8");
	CreateSceneComponent(Pt_9, "Pt_9");
	CreateSceneComponent(Pt_10, "Pt_10");
}


TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride)
{
	checkf(ImmutablePts.Num() >= NumPoints, TEXT("访问索引超过了数组的范围"));

	TArray<USceneComponent*> ArrayCopy;

	for(USceneComponent* Pt : ImmutablePts)
	{
		if(ArrayCopy.Num() >= NumPoints) return ArrayCopy;

		if(Pt != Pt_0)
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation(); //获取到节点基于根组件世界坐标系下的偏移
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector); //对偏移值进行垂直偏移
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint); //设置偏移后的坐标
		}

		//创建拾取坐标使用的起始点和最终位置
		const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 300.f);
		const FVector LoweredLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 300.f);

		//获取到周围过滤的对象
		TArray<AActor*> IgnoreActors;
		ULPAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

		//创建一条直线来拾取地面的坐标
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		//通过结果修改节点的位置和朝向
		const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
		Pt->SetWorldLocation(AdjustedLocation);
		Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		//添加到返回数组内
		ArrayCopy.Add(Pt);
	}

	return ArrayCopy;
}

void APointCollection::CreateSceneComponent(TObjectPtr<USceneComponent>& Pt, const FName Name)
{
	Pt = CreateDefaultSubobject<USceneComponent>(Name);
	ImmutablePts.Add(Pt);
	Pt->SetupAttachment(GetRootComponent());
}


void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}

