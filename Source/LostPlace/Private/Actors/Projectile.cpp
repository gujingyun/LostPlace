// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "LostPlace/LostPlace.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	//初始化碰撞体
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere); //设置其为根节点，
	Sphere->SetCollisionObjectType(ECC_Projectile); //设置其碰撞类型为世界动态物体
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); //设置其忽略所有碰撞检测
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); //设置其与世界动态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); //设置其与世界静态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //设置其与Pawn类型物体产生重叠事件

	//创建发射组件
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f; //设置初始速度
	ProjectileMovement->MaxSpeed = 550.f; //设置最大速度
	ProjectileMovement->ProjectileGravityScale = 0.f; //设置重力影响因子，0为不受影响



}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereOverlap);
	//设置此物体的存在时间
	SetLifeSpan(LifeSpan);
	
	//添加一个音效，并附加到根组件上面，在技能移动时，声音也会跟随移动
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		PlayImpact();
		bHit = true;
	}
	Super::Destroyed();
}

void AProjectile::PlayImpact() const
{
	//播放音效
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	//播放粒子特效
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
}

void AProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (DamageEffectHandle == nullptr)
	{
		return;
	}
	if (!DamageEffectHandle.Data.IsValid() || DamageEffectHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}
	TSharedPtr<FGameplayEffectSpec> GameplayEffectSpec = DamageEffectHandle.Data;
	FGameplayEffectSpec* EffectSpec = GameplayEffectSpec.Get();
	FGameplayEffectContextHandle GameplayEffectContextHandle = EffectSpec->GetContext();
	AActor* EffectCauser = GameplayEffectContextHandle.GetEffectCauser();

	if (!ULPAbilitySystemLibrary::IsNotFriend(EffectCauser, OtherActor))
	{
		return;
	}
	if (!bHit)
	{
		PlayImpact();
		bHit = true;
	}
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectHandle.Data.Get());
		}
		
		Destroy();
	}else
	{
		bHit = true;
	}
}


